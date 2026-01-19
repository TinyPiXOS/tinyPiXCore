#include "TpApp.h"
#include "TpString.h"
#include "TpUuid.h"

int32_t main(int32_t argc, char *argv[])
{
    TpUuid uuid0 = TpUuid::createUuid();
    TpUuid uuid1 = TpUuid::createUuidV1();
    TpUuid uuid4 = TpUuid::createUuidV4();

    std::cout << "uuid v0 string : " << uuid0.toString() << std::endl;
    std::cout << "uuid v0 base62 : " << uuid0.toBase62() << std::endl;
    std::cout << "uuid v0 pretty : " << uuid0.toPretty() << std::endl
              << std::endl;

    std::cout << "uuid v1 string : " << uuid1.toString() << std::endl;
    std::cout << "uuid v1 base62 : " << uuid1.toBase62() << std::endl;
    std::cout << "uuid v1 pretty : " << uuid1.toPretty() << std::endl
              << std::endl;

    std::cout << "uuid v4 string : " << uuid4.toString() << std::endl;
    std::cout << "uuid v4 base62 : " << uuid4.toBase62() << std::endl;
    std::cout << "uuid v4 pretty : " << uuid4.toPretty() << std::endl
              << std::endl;

    return 0;
}
