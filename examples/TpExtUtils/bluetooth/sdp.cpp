#include <iostream>
#include <stdio.h>
#include "TpApp.h"
#include "TpMainWindow.h"
#include "TpBluetoothServiceDiscovery.h"
#include "TpBluetoothService.h"
#include "TpBluetoothUuid.h"
#include "TpBluetoothLocal.h"


//获取蓝牙适配器的uuid列表
int example_bluet_get_uuids()
{
	TpBluetoothUuid uuid_serial(TpBluetoothUuid::TP_PROFILE_SERIAL_PORT);

	TpBluetoothLocal local("hci0");
	TpList<TpBluetoothUuid> uuid_list=local.getUuids();
	for(auto&it : uuid_list)
	{
		printf("uuid128:%s , Name:%s\n",it.toString().c_str(),it.toName().c_str());
	}
	if(local.isHaveUuid(uuid_serial))
	{
		printf("have uuid serial");
		return 1;
	}
	return 0;
}

int example_service_registe()
{
	TpBluetoothService service_serial;
	service_serial.setServiceChannel(1);
	service_serial.setServiceUuid(TpBluetoothUuid(TpBluetoothUuid::TP_PROFILE_SERIAL_PORT));
	service_serial.setServiceName(TpString("Serial Port Profile"));
	TpBluetoothUuid uuid(TpBluetoothUuid::TP_PROFILE_SERIAL_PORT);

	service_serial.registerService();
	while(1);
	return 0;
}


int example_bluet_uuid()
{
	tpUInt16 num_other=0x1205;
	tpUInt16 num_16=0x1101;
	tpUInt32 num_32=0x00001101;
	TpBluetoothUuid uuid_16(num_16);
	TpBluetoothUuid uuid_32(num_32);
	TpBluetoothUuid uuid_str(TpString("00001101-0000-1000-8000-00805F9B34FB"));
	TpBluetoothUuid uuid_p(TpBluetoothUuid::TP_PROFILE_SERIAL_PORT);
	printf("ok\n");

	printf("uuid_32:%08x\n",uuid_str.toUInt32());
	printf("uuid_16:%04x\n",uuid_str.toUInt16());
	printf("uuid128:%s\n",uuid_str.toString().c_str());
	printf("uuid name:%s\n",uuid_str.toName().c_str());

	if(uuid_16==uuid_32)
		printf("uuid_16==uuid_32\n");
	if(uuid_16==uuid_str)
		printf("uuid_16==uuid_str\n");
	if(uuid_16==uuid_p)
		printf("uuid_16==uuid_p\n");

	uuid_str=num_other;
	printf("uuid_32:%08x\n",uuid_str.toUInt32());
	printf("uuid_16:%04x\n",uuid_str.toUInt16());
	printf("uuid128:%s\n",uuid_str.toString().c_str());
	return 0;
}

//测试蓝牙服务的列表
void printSequence(const TpBluetoothService::Sequence& seq, const std::string& name = "Sequence") {
    std::cout << name << " (" << seq.size() << " elements):" << std::endl;
    for (size_t i = 0; i < seq.size(); i++) {
        const TpVariant& var = seq.at(i);
        std::cout << "  [" << i << "]: ";
        
        if (var.isBool()) {
            std::cout << "bool: " << std::boolalpha << var.toBool();
        }
        else if (var.isInt32()) {
            std::cout << "int32: " << var.toInt32();
        }
        else if (var.isUint32()) {
            std::cout << "uint32: " << var.toUInt32();
        }
        else if (var.isInt64()) {
            std::cout << "int64: " << var.toInt64();
        }
        else if (var.isUint64()) {
            std::cout << "uint64: " << var.toUint64();
        }
        else if (var.isFloat()) {
            std::cout << "float: " << var.toFloat();
        }
        else if (var.isDouble()) {
            std::cout << "double: " << var.toDouble();
        }
        else if (var.isString()) {
            std::cout << "string: \"" << var.toString() << "\"";
        }
        else if (var.isUint8()) {
            std::cout << "uint8: " << static_cast<int>(var.toUInt8());
        }
        else if (var.isUint16()) {
            std::cout << "uint16: " << var.toUInt16();
        }
        else if (var.isVector()) {
            std::cout << "vector (size: " << var.toVectorPtr()->size() << ")";
        }
        else {
            std::cout << "unknown type";
        }
        
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void printServiceDescriptor(const TpBluetoothService& service) {
    std::cout << "===== Service Descriptor =====" << std::endl;
    
    // 1. 基础信息
    std::cout << "Service Name: " << service.getServiceName() << std::endl;
    std::cout << "Service Description: " << service.getServiceDescription() << std::endl;
    std::cout << "Service Provider: " << service.getServiceProvider() << std::endl;
    std::cout << "Service RecHandle: " << std::hex<< service.getServiceRecHandle() << std::endl;
    // 2. 服务类 UUID 列表
    std::cout << "Service Class UUID List:" << std::endl;
    TpList<TpBluetoothUuid> classList = service.getServiceClassUuids();
    for (auto &it : classList) {
        std::cout << "  0x" << std::hex << it.toUInt16() << " (" << it.toName() << ")" << std::endl;
    }
    
    // 3. 协议描述符列表（专门处理）
    std::cout << "Protocol Descriptor List:" << std::endl;
    const TpBluetoothService::Sequence protocolList = service.getProtocolDescriptorList();
    for (size_t i = 0; i < protocolList.size(); i++) {
        TpBluetoothService::Sequence protocol = protocolList.sequenceValueAt(i);
        if (!protocol.isEmpty()) {
            // 协议 UUID
            if (protocol.at(0).isCustom<TpBluetoothUuid>()) {
                TpBluetoothUuid uuid = protocol.uuidValueAt(0);
                std::cout << "  Protocol: " << uuid.toString() << std::endl;
                
                // 特定协议处理
                if (uuid == TpBluetoothUuid(TpBluetoothUuid::TP_PROTOCOL_UUID_RFCOMM) && protocol.size() > 1) {
                    if (protocol.at(1).isUint8()) {
                        std::cout << "    RFCOMM Channel: " 
                                  << static_cast<int>(protocol.uint8ValueAt(1)) << std::endl;
                    }
                } else if (uuid == TpBluetoothUuid(TpBluetoothUuid::TP_PROTOCOL_UUID_L2CAP) && protocol.size() > 1) {
                    if (protocol.at(1).isUint16()) {
                        std::cout << "    L2CAP PSM: 0x" 
                                  << std::hex << protocol.uint16ValueAt(1) << std::dec << std::endl;
                    }
                }
            }
        }
    }
    
    // 4. 配置文件描述符列表
    std::cout << "Profile Descriptor List:" << std::endl;
    const TpBluetoothService::Sequence profileList = service.getProfileDescriptorList();
    for (size_t i = 0; i < profileList.size(); i++) {
        TpBluetoothService::Sequence profile = profileList.sequenceValueAt(i);
        if (profile.size() >= 2) {
            // 配置文件 UUID
            if (profile.at(0).isCustom<TpBluetoothUuid>()) {
                TpBluetoothUuid uuid = profile.uuidValueAt(0);
                std::cout << "  Profile: " << uuid.toString() << " (" << uuid.toName() << ")" << std::endl;
            }
            
            // 配置文件版本
            if (profile.at(1).isUint16()) {
                uint16_t version = profile.uint16ValueAt(1);
                uint8_t major = (version >> 8) & 0xFF;
                uint8_t minor = version & 0xFF;
                std::cout << "    Version: " << static_cast<int>(major) << "." 
                          << static_cast<int>(minor) << " (0x"
                          << std::hex << version << std::dec << ")" << std::endl;
            }
        }
    }
    
    // 5. 语言属性
   	std::cout << "Language Base Attribute ID List:" << std::endl;
    const TpVariant& langAttrVar = service.getAttribute(
        TpBluetoothService::LanguageBaseAttributeIdList);
	TpVariant nonConstLangAttrVar = langAttrVar;

    if (!nonConstLangAttrVar.isNull() && langAttrVar.isCustom<TpBluetoothService::Sequence>()) {
        const TpBluetoothService::Sequence langList = langAttrVar.toCustom<TpBluetoothService::Sequence>();
        
        for (size_t i = 0; i < langList.size(); i++) {
            const TpVariant& langVar = langList.at(i);
            
            if (langVar.isCustom<TpBluetoothService::Sequence>()) {
                const TpBluetoothService::Sequence langAttr = langVar.toCustom<TpBluetoothService::Sequence>();
                
                if (langAttr.size() >= 3) {
                    std::cout << "  Language ID: 0x" << std::hex << langAttr.uint16ValueAt(0) << std::dec << std::endl;
                    std::cout << "    Encoding: 0x" << std::hex << langAttr.uint16ValueAt(1) << std::dec << std::endl;
                    std::cout << "    Base Offset: 0x" << std::hex << langAttr.uint16ValueAt(2) << std::dec << std::endl;
                }
            }
        }
    } else {
        std::cout << "  Language Base Attribute ID List is not a sequence" << std::endl;
    }

    // 6. 其他属性
//    std::cout << "Service Record Handle: 0x" << std::hex << service.getServiceRecHandle() << std::dec << std::endl;
    std::cout << "Service Availability: " << static_cast<int>(
        service.getAttribute(TpBluetoothService::ServiceAvailability).toUInt8()) << std::endl;
    
    std::cout << "===== End of Service Descriptor =====" << std::endl;
}

void testServiceDescriptorPrinting() {
	std::cout << "===== Bluetooth Service Descriptor Printing Test =====" << std::endl;
    
    // 创建蓝牙服务对象
    TpBluetoothService service;
    
    // 设置服务记录句柄 (0x0000)
    service.setServiceRecHandle(0x10013);
    
    // 设置服务名称 (0x0100)
    service.setServiceName("Serial Port Profile");
    
    // 设置服务类 UUID 列表 (0x0001)
   	TpList<TpBluetoothUuid> classIdList;
   	classIdList.emplace_back(TpBluetoothUuid(TpBluetoothUuid::TP_PROFILE_SERIAL_PORT));
    service.setServiceClassUuids(classIdList);
    
    // 设置协议描述符列表 (0x0004)
    TpBluetoothService::Sequence protocolList;
    
    // L2CAP 协议
    TpBluetoothService::Sequence l2capProtocol;
    l2capProtocol << TpBluetoothUuid(TpBluetoothUuid::TP_PROTOCOL_UUID_L2CAP)
                 << static_cast<uint16_t>(0x0001); // PSM
    
    // RFCOMM 协议
    TpBluetoothService::Sequence rfcommProtocol;
    rfcommProtocol << TpBluetoothUuid(TpBluetoothUuid::TP_PROTOCOL_UUID_RFCOMM)
                  << static_cast<uint8_t>(1); // 通道号
    
    protocolList << l2capProtocol << rfcommProtocol;
    service.setProtocolDescriptorList(protocolList);
    
    // 设置配置文件描述符列表 (0x0009)
    TpBluetoothService::Sequence profileList;
    
    // 串口配置文件
    TpBluetoothService::Sequence sppProfile;
    sppProfile << TpBluetoothUuid(TpBluetoothUuid::TP_PROFILE_SERIAL_PORT)
              << static_cast<uint16_t>(0x0102); // 版本号 1.2
    
    profileList << sppProfile;
    service.setProfileDescriptorList(profileList);
    
    // 设置语言属性列表 (0x0006)
    TpBluetoothService::Sequence langList;
    TpBluetoothService::Sequence langAttr;
    langAttr << static_cast<uint16_t>(0x656E) // 英语
             << static_cast<uint16_t>(0x006A) // UTF-8 编码
             << static_cast<uint16_t>(0x0100); // 基础偏移
    langList << langAttr;
    service.setAttribute(TpBluetoothService::LanguageBaseAttributeIdList, langList);
    
    // 设置服务可用性 (0x0008)
    service.setAttribute(TpBluetoothService::ServiceAvailability, static_cast<uint8_t>(100));
    
    // 打印服务描述符
    printServiceDescriptor(service);
    
    std::cout << "===== Test Completed =====" << std::endl;
}


int example_service()
{
    std::cout << "===== Sequence Class Test =====" << std::endl;
    
    // 创建基本序列
    TpBluetoothService::Sequence basicSeq;
    basicSeq << true << 42 << 3.14f << "Hello World";
    printSequence(basicSeq, "Basic Sequence");
    
    // 测试 uint8_t 和 uint16_t 支持
    TpBluetoothService::Sequence numberSeq;
    numberSeq << static_cast<uint8_t>(8) 
              << static_cast<uint16_t>(16)
              << static_cast<uint8_t>(255)  // 测试边界值
              << static_cast<uint16_t>(65535); // 测试边界值
    printSequence(numberSeq, "Number Sequence");
    
    // 测试类型安全的取值方法
    try {
        std::cout << "Testing type-safe access methods:" << std::endl;
        uint8_t u8Val = numberSeq.uint8ValueAt(0);
        uint16_t u16Val = numberSeq.uint16ValueAt(1);
        std::cout << "  uint8ValueAt(0): " << static_cast<int>(u8Val) << std::endl;
        std::cout << "  uint16ValueAt(1): " << u16Val << std::endl;
        
        // 测试类型错误处理
        try {
            std::cout << "Attempting to get uint8 from uint16 position:" << std::endl;
            uint8_t badVal = numberSeq.uint8ValueAt(1);
            std::cout << "  This should not be printed: " << static_cast<int>(badVal) << std::endl;
        } catch (const std::exception& e) {
            std::cout << "  Caught expected exception: " << e.what() << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    std::cout << std::endl;
    
    // 测试嵌套序列
    TpBluetoothService::Sequence nestedSeq;
    nestedSeq << "Top Level" << numberSeq << 3.14159;
    printSequence(nestedSeq, "Nested Sequence");
    
    // 测试序列取值方法
    try {
        std::cout << "Testing sequenceValueAt method:" << std::endl;
        TpBluetoothService::Sequence extractedSeq = nestedSeq.sequenceValueAt(1);
        printSequence(extractedSeq, "Extracted Sequence");
        
        // 测试嵌套序列中的值
        uint8_t nestedU8 = extractedSeq.uint8ValueAt(0);
        uint16_t nestedU16 = extractedSeq.uint16ValueAt(1);
        std::cout << "  Nested uint8: " << static_cast<int>(nestedU8) << std::endl;
        std::cout << "  Nested uint16: " << nestedU16 << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    std::cout << std::endl;
    
    // 测试蓝牙协议描述符
    std::cout << "===== Bluetooth Protocol Descriptor Test =====" << std::endl;
    
    // 创建 L2CAP 协议描述符
    TpBluetoothService::Sequence l2capProtocol;
    l2capProtocol << "L2CAP" << static_cast<uint16_t>(0x0001); // PSM
    
    // 创建 RFCOMM 协议描述符
    TpBluetoothService::Sequence rfcommProtocol;
    rfcommProtocol << "RFCOMM" << static_cast<uint8_t>(6); // 通道号
    
    // 创建协议描述符列表
    TpBluetoothService::Sequence protocolList;
    protocolList << l2capProtocol << rfcommProtocol;
    printSequence(protocolList, "Protocol Descriptor List");
    
    // 访问协议描述符
    try {
        // 获取 L2CAP 协议
        TpBluetoothService::Sequence l2cap = protocolList.sequenceValueAt(0);
        std::string protocolName = l2cap.stringValueAt(0);
        uint16_t psm = l2cap.uint16ValueAt(1);
        std::cout << "L2CAP Protocol:" << std::endl;
        std::cout << "  Name: " << protocolName << std::endl;
        std::cout << "  PSM: 0x" << std::hex << psm << std::dec << std::endl;
        
        // 获取 RFCOMM 协议
        TpBluetoothService::Sequence rfcomm = protocolList.sequenceValueAt(1);
        protocolName = rfcomm.stringValueAt(0);
        uint8_t channel = rfcomm.uint8ValueAt(1);
        std::cout << "RFCOMM Protocol:" << std::endl;
        std::cout << "  Name: " << protocolName << std::endl;
        std::cout << "  Channel: " << static_cast<int>(channel) << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    std::cout << std::endl;
    
    // 测试服务属性设置
    TpBluetoothService service;
    
    // 设置协议描述符列表
    service.setProtocolDescriptorList(protocolList);
    std::cout << "Protocol Descriptor List set in service" << std::endl;
    
    // 获取协议描述符列表
    TpBluetoothService::Sequence retrievedList = service.getProtocolDescriptorList();
    printSequence(retrievedList, "Retrieved Protocol Descriptor List");
    
    // 比较原始和检索的序列
    if (retrievedList.size() == protocolList.size()) {
        std::cout << "Retrieved sequence has same size as original" << std::endl;
    } else {
        std::cout << "WARNING: Retrieved sequence size differs from original" << std::endl;
    }
    
    std::cout << "===== Test Completed =====" << std::endl;
    
    return 0;
}




int example_service_scan(TpApp& app,bool signal)
{

	TpBluetoothServiceDiscovery scan(TpBluetoothAddress("00:11:22:33:44:55"));
	scan.setRemoteAddress(TpBluetoothAddress(TpString("E4:5F:01:37:58:93")));
	scan.discoveryServices();
	if(signal)
	{
		connect(&scan,finished,[=](TpList<TpBluetoothService>& services){
			std::cout << "扫描完成====================================================\n";
			for(auto &it : services)
			{
				printServiceDescriptor(it);
			}
		});
	}
	else{
		while(scan.isDiscovering())	//等待扫描完成
		{
			usleep(1000);
		}
		TpList<TpBluetoothService> services=scan.getDiscoveredServices();
		printf("====================================================\n");
		for(auto &it : services)
		{
			printServiceDescriptor(it);
		}
	}
	app.run();
	return 0;
}

int main(int argc,char *argv[])
{
	TpApp app(argc, argv);
	TpMainWindow *vScreen = new TpMainWindow();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true); // vScreen setvisible will be update display weekly
	app.bindVScreen(vScreen);
//	example_service();
//	testServiceDescriptorPrinting();
//	example_bluet_uuid();
//	example_bluet_get_uuids();
	example_service_scan(app,true);
//	example_service_registe();
	return 0;
}