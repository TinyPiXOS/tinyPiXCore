#ifndef _TP_BLUETOOTH_UUID_H_
#define _TP_BLUETOOTH_UUID_H_

#include <TpCore.h>

TP_DEF_VOID_TYPE_VAR(ITpBluetoothUuidData);


class TpBluetoothUuid{
public:
	enum Format{
		Format16Bit = 0,
		Format32Bit,
		Format128Bit,
		FormatUnknown
	};
	enum ProtocolUuid{
		TP_PROTOCOL_UUID_SDP   					= 0x0001,
		TP_PROTOCOL_UUID_UDP   					= 0x0002,
		TP_PROTOCOL_UUID_RFCOMM 				= 0x0003,
		TP_PROTOCOL_UUID_TCP       				= 0x0004,
		TP_PROTOCOL_UUID_TCS_BIN				= 0x0005,
		TP_PROTOCOL_UUID_TCS_AT   				= 0x0006,
		TP_PROTOCOL_UUID_OBEX   				= 0x0008,
		TP_PROTOCOL_UUID_IP      				= 0x0009,
		TP_PROTOCOL_UUID_FTP    				= 0x000A,
		TP_PROTOCOL_UUID_HTTP  					= 0x000C,
		TP_PROTOCOL_UUID_WSP    				= 0x000E,
		TP_PROTOCOL_UUID_BNEP   				= 0x000F,/* PAN */
		TP_PROTOCOL_UUID_HIDP   				= 0x0011, /* HID */
		TP_PROTOCOL_UUID_HARDCOPY_CONTROL_CHANNEL = 0x0012, /* HCRP */
		TP_PROTOCOL_UUID_HARDCOPY_DATA_CHANNEL 	= 0x0014, /* HCRP */
		TP_PROTOCOL_UUID_HARDCOPY_NOTIFICATION	= 0x0016, /* HCRP */
		TP_PROTOCOL_UUID_AVCTP  				= 0x0017, /* AVCTP */
		TP_PROTOCOL_UUID_AVDTP  				= 0x0019, /* AVDTP */
		TP_PROTOCOL_UUID_CMTP  					= 0x001B, /* CIP */
		TP_PROTOCOL_UUID_UDI_C_PLANE 			= 0x001D, /* UDI */
		TP_PROTOCOL_UUID_L2CAP  				= 0x0100
	};
	enum ProfileUuid{
		TP_PROFILE_SERVICE_DISCOVERY_SERCER	= 0x1000,//ServiceDiscoveryServer", NULL
		TP_PROFILE_BROWSE_GROUP_DESCRIPTOR	= 0x1001,//BrowseGroupDescriptor", NULL
		TP_PROFILE_PUBLIC_BROWSE_GROUP	= 0x1002,//PublicBrowseGroup", NULL
		TP_PROFILE_SERIAL_PORT	= 0x1101,//SerialPort", "Serial"
		TP_PROFILE_LAN_ACCESS_USING_PPP	= 0x1102,//LANAccessUsingPPP", NULL
		TP_PROFILE_DIALUP_NETWORKING	= 0x1103,//DialupNetworking", "DUN"
		TP_PROFILE_IRMC_SYNC	= 0x1104,//IrMCSync", NULL
		TP_PROFILE_OBEX_OBJECT_PUSH	= 0x1105,//OBEXObjectPush", NULL
		TP_PROFILE_OBEX_FILE_TRANSFER	= 0x1106,//OBEXFileTransfer", NULL
		TP_PROFILE_IRMC_SYNC_COMMAND	= 0x1107,//IrMCSyncCommand", NULL
		TP_PROFILE_HEADSET	= 0x1108,//Headset", NULL
		TP_PROFILE_CORDLESS_TELEPHONY	= 0x1109,//CordlessTelephony", NULL
		TP_PROFILE_AUDIO_SOURCE	= 0x110A,//AudioSource", NULL
		TP_PROFILE_AUDIO_SINK	= 0x110B,//AudioSink", NULL
		TP_PROFILE_AV_REMOTE_CONTROL_TARGET	= 0x110C,//AVRemoteControlTarget", NULL
		TP_PROFILE_ADVANCED_AUDIO_DISTRIBUTION	= 0x110D,//AdvancedAudioDistribution", "A2DP"
		TP_PROFILE_AV_REMOTE_CONTROL	= 0x110E,//AVRemoteControl", NULL
		TP_PROFILE_VIDEO_CONFERENCING	= 0x110F,//VideoConferencing", NULL
		TP_PROFILE_INTERCOM	= 0x1110,//Intercom", NULL
		TP_PROFILE_FAX	= 0x1111,//Fax", NULL
		TP_PROFILE_HEADSET_AUDIO_GATEWAY	= 0x1112,//HeadsetAudioGateway", NULL
		TP_PROFILE_WAP	= 0x1113,//WAP", NULL
		TP_PROFILE_WAP_CLIENT	= 0x1114,//WAPClient", NULL
		TP_PROFILE_PANU	= 0x1115,//PANU", NULL
		TP_PROFILE_NAP	= 0x1116,//NAP", NULL
		TP_PROFILE_GN	= 0x1117,//GN", NULL
		TP_PROFILE_DIRECT_PRINTING	= 0x1118,//DirectPrinting", NULL
		TP_PROFILE_REFERENCE_PRINTING	= 0x1119,//ReferencePrinting", NULL
		TP_PROFILE_IMAGING	= 0x111A,//Imaging", NULL},
		TP_PROFILE_IMAGING_RESPONDER	= 0x111B,//ImagingResponder", NULL
		TP_PROFILE_IMAGING_AUTOMATIC_ARCHIVE	= 0x111C,//ImagingAutomaticArchive", NULL
		TP_PROFILE_IMAGING_REFERENCE_OBJECTS	= 0x111D,//ImagingReferenceObjects", NULL
		TP_PROFILE_HANDSFREE	= 0x111E,//Handsfree", NULL},
		TP_PROFILE_HANDSFREE_AUDIO_GATEWAY	= 0x111F,//HandsfreeAudioGateway", NULL
		TP_PROFILE_DIRECT_PRINTING_REFERENCE_OBJECTS	= 0x1120,//DirectPrintingReferenceObjects", NULL
		TP_PROFILE_REFLECTED_UI	= 0x1121,//ReflectedUI", NULL
		TP_PROFILE_BASIC_PRINGING	= 0x1122,//BasicPringing", NULL
		TP_PROFILE_PRINTING_STATUS	= 0x1123,//PrintingStatus", NULL
		TP_PROFILE_HUMAN_INTERFACE_DEVICE	= 0x1124,//HumanInterfaceDevice", "HID"
		TP_PROFILE_HARDCOPY_CABLE_REPLACEMENT	= 0x1125,//HardcopyCableReplacement", NULL
		TP_PROFILE_HCR_PRINT	= 0x1126,//HCRPrint", NULL
		TP_PROFILE_HCR_SCAN	= 0x1127,//HCRScan", NULL
		TP_PROFILE_COMMON_ISDN_ACCESS	= 0x1128,//CommonISDNAccess", NULL
		TP_PROFILE_VIDEO_CONFERENCING_GW	= 0x1129,//VideoConferencingGW", NULL
		TP_PROFILE_UDIMT	= 0x112A,//UDIMT", NULL
		TP_PROFILE_UDITA	= 0x112B,//UDITA", NULL
		TP_PROFILE_AUDIO_VIDEO	= 0x112C,//AudioVideo", NULL
		TP_PROFILE_SIM_ACCESS	= 0x112D,//SIMAccess", "SAP"
		TP_PROFILE_PN_P_INFORMATION	= 0x1200,//PnPInformation", NULL
		TP_PROFILE_GENERIC_NETWORKING	= 0x1201,//GenericNetworking", NULL
		TP_PROFILE_GENERIC_FILE_TRANSFER	= 0x1202,//GenericFileTransfer", NULL
		TP_PROFILE_GENERIC_AUDIO	= 0x1203,//GenericAudio", NULL
		TP_PROFILE_GENERIC_TELEPHONY	= 0x1204,//GenericTelephony", NULL
		TP_PROFILE_UPN_P	= 0x1205,//UPnP", NULL
		TP_PROFILE_UPN_P_IP	= 0x1206,//UPnPIp", NULL
		TP_PROFILE_ESDP_UPN_P_IP_PAN	= 0x1300,//ESdpUPnPIpPan", NULL
		TP_PROFILE_ESDP_UPN_P_IP_LAP	= 0x1301,//ESdpUPnPIpLap", NULL
		TP_PROFILE_EDP_UPNP_IP_L2_CAP	= 0x1302,//EdpUPnpIpL2CAP", NULL

			// Custom:
		TP_PROFILE_PHONE_BOOK_ACCESS	= 0x112F,//PhoneBookAccess", NULL
	};
public:
	TpBluetoothUuid();
	TpBluetoothUuid(tpUInt16 uuid);
	TpBluetoothUuid(tpUInt32 uuid);
	TpBluetoothUuid(const tpUInt8 uuid[16]);
	TpBluetoothUuid(const TpString &uuid);
	TpBluetoothUuid(const TpBluetoothUuid::ProtocolUuid uuid);
	TpBluetoothUuid(const TpBluetoothUuid::ProfileUuid uuid);
	TpBluetoothUuid(const TpBluetoothUuid& uuid);
	~TpBluetoothUuid();

	TpBluetoothUuid& operator=(const TpBluetoothUuid &other);
	tpBool operator!=(const TpBluetoothUuid &other) const;
	tpBool operator==(const TpBluetoothUuid &other) const;
public:
	/// @brief 获取UUID类型
	/// @return 
	TpBluetoothUuid::Format getFormat() const;

	/// @brief 转换为tpUInt16类型
	/// @param ok 是否成功的返回
	/// @return tpUInt16类型UUID
	tpUInt16 toUInt16(tpBool *ok = nullptr)const;

	/// @brief 转换为tpUInt32类型
	/// @param ok 是否成功的返回
	/// @return toUInt32类型的UUID
	tpUInt32 toUInt32(tpBool *ok = nullptr)const;

	//TpUInt128 toUInt128()const;
	
	/// @brief 转换字符串类型
	/// @return 字符串类型UUID
	TpString toString()const;

	/// @brief UUID是否为空
	/// @return 
	tpBool isNull() const;

	TpString toName()const;

private:
	ITpBluetoothUuidData *data_;
};




#endif
