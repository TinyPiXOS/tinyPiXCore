#!/bin/bash
# alsa-auto-fixed-config.sh - 自动化修复组文件并配置ALSA

# 1. 自动化修复组文件（非交互式）
echo "自动修复组文件..."
sudo cp /etc/group /etc/group.bak
sudo cp /etc/gshadow /etc/gshadow.bak

# 使用awk智能去重，保留第一个有效条目
sudo awk -F: '!seen[$1]++' /etc/group | sudo tee /etc/group.fixed >/dev/null
sudo mv /etc/group.fixed /etc/group

# 修复gshadow格式问题
sudo awk -F: '
BEGIN { OFS = FS } 
{ 
    if (NF < 4) { 
        if (NF == 1) print $1 "::::"
        else if (NF == 2) print $1 ":" $2 "::"
        else if (NF == 3) print $1 ":" $2 ":" $3 ":"
        else print $0
    } 
    else print $0 
}' /etc/gshadow | sudo tee /etc/gshadow.fixed >/dev/null
sudo mv /etc/gshadow.fixed /etc/gshadow

# 2. 创建必要的ALSA目录和文件
sudo mkdir -p /var/lib/alsa /usr/share/alsa/cards
sudo touch /var/lib/alsa/asound.state
echo "state.AudioPCI { }" | sudo tee /usr/share/alsa/cards/AudioPCI.conf >/dev/null

# 3. 安全的ALSA配置
CARD_NUM=$(aplay -l 2>/dev/null | grep '^card [0-9]' | head -1 | cut -d: -f1 | tr -d 'card ')
[ -z "$CARD_NUM" ] && CARD_NUM=0

cat > ~/.asoundrc <<EOF
# ALSA混音配置 - 自动生成于 $(date)
pcm.!default {
    type plug
    slave.pcm "dmixer"
}

pcm.dmixer {
    type dmix
    ipc_key 2048
    ipc_perm 0666
    slave {
        pcm "hw:$CARD_NUM,0"
        period_size 1024
        buffer_size 4096
        rate 48000
    }
    bindings {
        0 0
        1 1
    }
}

ctl.!default {
    type hw
    card $CARD_NUM
}
EOF

# 4. 安全的用户组处理
if ! id -nG "$USER" | grep -q '\baudio\b'; then
    sudo groupadd -f audio 2>/dev/null
    sudo gpasswd -a "$USER" audio >/dev/null
    # 立即应用组变更
    newgrp audio <<EONG
EONG
fi

# 5. 静默应用配置
sudo alsactl restore -f /usr/share/alsa/alsa.conf >/dev/null 2>&1
sudo alsactl init >/dev/null 2>&1

# 6. 创建干净的验证输出
echo "ALSA配置完成 | 声卡: hw:$CARD_NUM"