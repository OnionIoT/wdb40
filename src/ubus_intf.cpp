#include <ubus_intf.h>

extern "C" {
#include "libubus.h"
#include <blobmsg_intf.h>
}

//// static members
struct blob_attr* 	ubusIntf::receivedAttribute;
char* 				ubusIntf::attributeName;
char* 				ubusIntf::attributeValue;
int 				ubusIntf::attributeType;


// constructor
ubusIntf::ubusIntf(void)
{
	Reset();
}

// destructor
ubusIntf::~ubusIntf(void)
{
	// nothing for now
}


void ubusIntf::Reset()
{
	receivedAttribute 	= NULL;
	attributeName 		= NULL;
	attributeValue 		= NULL;

	ctx 				= NULL;
	ubus_socket 		= NULL;

	bBackendInitialized 	= 0;
	timeout 				= UBUS_INTF_DEFAULT_TIMEOUT;
}


// Allocate the UBUS context
int ubusIntf::ReadContext()
{
	int status;

	// initialization of ubus context
	ctx 	= ubus_connect(ubus_socket);

	if (!ctx) {
		bBackendInitialized = 0;
		return EXIT_FAILURE;
	}

	bBackendInitialized = 1;
	return EXIT_SUCCESS;
}

// free the uci context
int ubusIntf::ReleaseContext()
{
	if (ctx != NULL && bBackendInitialized == 1) {
		bBackendInitialized = 0;
		ubus_free (ctx);

		return EXIT_SUCCESS;
	}

	return EXIT_FAILURE;
}


//// handler function
void _receiveCallResultData(struct ubus_request *req, int type, struct blob_attr *msg)
{
	int status;

	// check for valid response
	if (!msg) {
		return;
	}

	// read the blobMsg for the attribute specified by the static member
	//status 	= blobMsgFindAttr(msg, ubusIntf::attributeName, ubusIntf::receivedAttribute);
	ubusIntf::attributeValue 	= new char[IWINFO_MAX_STRING_SIZE];
	status 	= blobMsgFindValue(msg, ubusIntf::attributeName, ubusIntf::attributeValue, &(ubusIntf::attributeType) );
}


// check the network.wireless status
int ubusIntf::GetNetworkWirelessUpStatus(int &bUp)
{
	int 	status;

	uint32_t	 	methodId;
	std::string		group, method;


	// get the context
	_Print(4, "UBUS DBG: about to read context\n");
	status 	= ReadContext();
	if (status != EXIT_SUCCESS) {
		_Print(1, "> ERROR: Could not allocate ubus context\n");
		return EXIT_FAILURE;
	}

	// populate the group and method
	group 	= UBUS_INTF_NETWORK_WIRELESS_PATH;
	method 	= UBUS_INTF_NETWORK_WIRELESS_METHOD_STATUS;

	// lookup ubus group
	_Print(4, "UBUS DBG: about to lookup id\n");
	status 	= ubus_lookup_id(ctx, group.c_str(), &methodId);

	if (status == EXIT_SUCCESS) {
		// initialize the msg
		_Print(4, "UBUS DBG: blob_buf_init\n");
		blob_buf_init(&bMsg, 0);

		// setup the static variables for the handler
		_Print(4, "UBUS DBG: attributeName allocation\n");
		attributeName 	= new char[IWINFO_MAX_STRING_SIZE];
		sprintf(attributeName, "radio0/%s", UBUS_INTF_NETWORK_WIRELESS_STATUS_ATTRIBUTE_UP);

		_Print(4, "UBUS DBG: about to ubus_invoke\n");
		status 	= ubus_invoke(	ctx, methodId, method.c_str(), 					// ubus context, group id, method string
								bMsg.head, _receiveCallResultData, NULL,		// blob attr, handler function, priv
								UBUS_INTF_DEFAULT_TIMEOUT*UBUS_INTF_TIMEOUT_MULTIPLIER);	// timeout

		_Print(4, "UBUS DBG: about to read booleanValue\n");
		status 	= _ReadBooleanValue(bUp);

		// cleanup
		_Print(4, "UBUS DBG: blob_buf_free\n");
		blob_buf_free(&bMsg);
		delete attributeName;
	}

	// release the context
	status 	|= ReleaseContext();

	return 	status;
}

int ubusIntf::_ReadBooleanValue(int &bVal) 
{
	int status;

	// check for valid attribute value
	if (attributeValue == NULL) {
		return EXIT_FAILURE;
	}

	// check the value
	if (strcmp(attributeValue, UBUS_INTF_VALUE_TRUE) == 0 ) {
		// value is true
		bVal 	= 1;
	}
	else if (strcmp(attributeValue, UBUS_INTF_VALUE_FALSE) == 0 ) {
		// value is false
		bVal 	= 0;
	}
	else {
		// invalid value
		bVal 	= 0;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
