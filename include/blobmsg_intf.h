#ifndef _BLOBMSG_INTF_H_
#define _BLOBMSG_INTF_H_


#include <libubox/blobmsg_json.h>


// define for debug prints
//#define BLOBMSG_INTF_DEBUG

#ifdef BLOBMSG_INTF_DEBUG
	#define BLOBMSG_DBG_PRINT(a,...) 			printf(a,##__VA_ARGS__)
#else
	#define BLOBMSG_DBG_PRINT(a,...)
#endif



#define BLOBMSG_INTF_SEARCH_DELIMITER		"/"

//// blobmsg handling handling functions
void 		blobMsgTraverse		(struct blob_attr *attr);

// based on an attribute name, return the value as a char* and the value type as an int
int  		blobMsgFindValue	(struct blob_attr *attr, char* name, char* val, int *valType);

// based on an attribute name, return a pointer to the blob_attr struct
int 		blobMsgFindAttr 	(struct blob_attr *attr, char* name, struct blob_attr *ret);


// convert a blob_attr to a char type
int  		blobAttrValToChar	(struct blob_attr *attr, char* ret, int *type);

#endif // _BLOBMSG_INTF_H_