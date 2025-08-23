#include <iostream>
#include <stdio.h>
#include "tpBluetoothServiceDiscovery.h"
#include "tpBluetoothService.h"
#include "tpBluetoothUuid.h"
#include "tpBluetoothLocal.h"


//获取蓝牙适配器的uuid列表
int example_bluet_get_uuids()
{
	tpBluetoothUuid uuid_serial(tpBluetoothUuid::TP_PROFILE_SERIAL_PORT);

	tpBluetoothLocal local("hci0");
	tpList<tpBluetoothUuid> uuid_list=local.getUuids();
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
	tpBluetoothService service_serial;
	service_serial.setServiceChannel(1);
	service_serial.setServiceUuid(tpBluetoothUuid(tpBluetoothUuid::TP_PROFILE_SERIAL_PORT));
	service_serial.setServiceName(tpString("Serial Port Profile"));
	tpBluetoothUuid uuid(tpBluetoothUuid::TP_PROFILE_SERIAL_PORT);

	service_serial.registerService();
	while(1);
	return 0;
}


int example_service_scan()
{
	tpBluetoothServiceDiscovery scan(tpBluetoothAddress("00:11:22:33:44:55"));
	scan.start();
	return 0;
}

int example_bluet_uuid()
{
	tpUInt16 num_other=0x1205;
	tpUInt16 num_16=0x1101;
	tpUInt32 num_32=0x00001101;
	tpBluetoothUuid uuid_16(num_16);
	tpBluetoothUuid uuid_32(num_32);
	tpBluetoothUuid uuid_str(tpString("00001101-0000-1000-8000-00805F9B34FB"));
	tpBluetoothUuid uuid_p(tpBluetoothUuid::TP_PROFILE_SERIAL_PORT);
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
void printSequence(const tpBluetoothService::Sequence& seq, const std::string& name = "Sequence") {
    std::cout << name << " (" << seq.size() << " elements):" << std::endl;
    for (size_t i = 0; i < seq.size(); i++) {
        const tpVariant& var = seq.at(i);
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

int example_service()
{
    std::cout << "===== Sequence Class Test =====" << std::endl;
    
    // 创建基本序列
    tpBluetoothService::Sequence basicSeq;
    basicSeq << true << 42 << 3.14f << "Hello World";
    printSequence(basicSeq, "Basic Sequence");
    
    // 测试 uint8_t 和 uint16_t 支持
    tpBluetoothService::Sequence numberSeq;
    numberSeq << static_cast<uint8_t>(8) 
              << static_cast<uint16_t>(16)
              << static_cast<uint8_t>(255)  // 测试边界值
              << static_cast<uint16_t>(65535); // 测试边界值
    printSequence(numberSeq, "Number Sequence");
    
    // 测试类型安全的取值方法
    try {
        std::cout << "Testing type-safe access methods:" << std::endl;
        uint8_t u8Val = numberSeq.uint32ValueAt(0);
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
    tpBluetoothService::Sequence nestedSeq;
    nestedSeq << "Top Level" << numberSeq << 3.14159;
    printSequence(nestedSeq, "Nested Sequence");
    
    // 测试序列取值方法
    try {
        std::cout << "Testing sequenceValueAt method:" << std::endl;
        tpBluetoothService::Sequence extractedSeq = nestedSeq.sequenceValueAt(1);
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
    tpBluetoothService::Sequence l2capProtocol;
    l2capProtocol << "L2CAP" << static_cast<uint16_t>(0x0001); // PSM
    
    // 创建 RFCOMM 协议描述符
    tpBluetoothService::Sequence rfcommProtocol;
    rfcommProtocol << "RFCOMM" << static_cast<uint8_t>(6); // 通道号
    
    // 创建协议描述符列表
    tpBluetoothService::Sequence protocolList;
    protocolList << l2capProtocol << rfcommProtocol;
    printSequence(protocolList, "Protocol Descriptor List");
    
    // 访问协议描述符
    try {
        // 获取 L2CAP 协议
        tpBluetoothService::Sequence l2cap = protocolList.sequenceValueAt(0);
        std::string protocolName = l2cap.stringValueAt(0);
        uint16_t psm = l2cap.uint16ValueAt(1);
        std::cout << "L2CAP Protocol:" << std::endl;
        std::cout << "  Name: " << protocolName << std::endl;
        std::cout << "  PSM: 0x" << std::hex << psm << std::dec << std::endl;
        
        // 获取 RFCOMM 协议
        tpBluetoothService::Sequence rfcomm = protocolList.sequenceValueAt(1);
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
    tpBluetoothService service;
    
    // 设置协议描述符列表
    service.setProtocolDescriptorList(protocolList);
    std::cout << "Protocol Descriptor List set in service" << std::endl;
    
    // 获取协议描述符列表
    tpBluetoothService::Sequence retrievedList = service.getProtocolDescriptorList();
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


int main()
{
	example_service();
//	example_bluet_uuid();
//	example_bluet_get_uuids();
//	example_service_scan();
//	example_service_registe();
	return 0;
}