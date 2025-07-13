#include "tpGateway.h"
#include <thread>

class TestRecvDataClass : public tpGateway
{
public:
	virtual void recvData(const char *topic, const void *data, const uint32_t &size)
	{
		int32_t recvData = *(int32_t*)data;
		std::cout << "Topic : " << topic << std::endl;
		std::cout << "RecvData : " << recvData << std::endl;
		std::cout << "DataSize : " << size << std::endl;
	}
};

int32_t main(int32_t argc, char *argv[])
{
	TestRecvDataClass *recvClass = new TestRecvDataClass();

	initializeGateway();
	subscribeGatewayData("TestTopic22", recvClass);

	while (1)
	{
		std::cout << "PublishData " << std::endl;
		int32_t pubData = 1;
		publishGatewayData("TestTopic", &pubData, sizeof(int32_t));
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	shutdownGateway();
	
	return 0;
}
