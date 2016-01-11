#include <blobmsg_intf.h>
#include <string.h>
#include <stdio.h>



//// private function prototypes
void 		_traverse 			(struct blob_attr *attr, int len);
void*		_searchTraverse 	(struct blob_attr *attr, int len, char* name);



/// traverse and print the blobmsg contents
void blobMsgTraverse(struct blob_attr *attr)
{
	bool 		array;

	array = 	blob_is_extended(attr) &&
				blobmsg_type(attr) == BLOBMSG_TYPE_ARRAY;

	// traverse
	_traverse(blobmsg_data(attr), blobmsg_data_len(attr));
}

void _traverse(struct blob_attr *attr, int len)
{
	struct 	blob_attr 	*pos;
	int 	type;

	// traverse each item in the blob (at this level)
	__blob_for_each_attr(pos, attr, len) {
		// find the type
		type 	= blobmsg_type(pos);
		printf("item:: name: %s, type: %d\n", blobmsg_name(pos), type );

		// descend into arrays and tables
		if 	(	type == BLOBMSG_TYPE_TABLE ||
				type == BLOBMSG_TYPE_ARRAY
			)
		{
			_traverse(blobmsg_data(pos), blobmsg_data_len(pos) );
		}
	}
}



/// traverse the blobmsg contents, looking for a particular attribue
// LAZAR: ret does not get changed, some pointer fuckery...
int blobMsgFindAttr(struct blob_attr *attr, char* name, struct blob_attr *ret)
{
	struct blob_attr 	*var 	= NULL;

	// traverse the blobmsg searching for the value
	BLOBMSG_DBG_PRINT("> Searching blobMsg for \"%s\"\n", name);
	ret 	= _searchTraverse( blobmsg_data(attr), blobmsg_data_len(attr), name );

	//(*ret) 	= (*var);
	if (ret == NULL) {
		BLOBMSG_DBG_PRINT("blobMsgFindAttr:: ret is NULL\n");
	}
	else {
		BLOBMSG_DBG_PRINT("blobMsgFindAttr:: ret is ok\n");
	}

	return (var != NULL ? EXIT_SUCCESS : EXIT_FAILURE);
}

int blobMsgFindValue(struct blob_attr *attr, char* name, char* val, int *valType)
{
	int 	status;
	struct 	blob_attr 	*var = NULL;

	// traverse the blobmsg searching for the value
	BLOBMSG_DBG_PRINT("> Searching blobMsg for \"%s\"\n", name);
	var 	= _searchTraverse( blobmsg_data(attr), blobmsg_data_len(attr), name );

	// convert the find result to a char*
	blobAttrValToChar(var, val, valType);

	// check the results
	if (var == NULL) {
		BLOBMSG_DBG_PRINT("> No match found!\n");
		*valType 	= (int)BLOBMSG_TYPE_UNSPEC;
		return EXIT_FAILURE;
	}

	BLOBMSG_DBG_PRINT("> Found var \"%s\", type: %d, value: \"%s\"\n", blobmsg_name(var), blobmsg_type(var), val );

	return EXIT_SUCCESS;
}

void* _searchTraverse(struct blob_attr *attr, int len, char* name)
{
	struct 	blob_attr 	*pos, *ret;
	char* 	currentName;
	int 	type, value;

	// find the current variable name
	currentName 	= strtok(name, BLOBMSG_INTF_SEARCH_DELIMITER);
	if (currentName == NULL) {
		return NULL;
	}
	//BLOBMSG_DBG_PRINT(">> Current token:  \"%s\"\n", currentName);


	// traverse each item in the blob (at this level)
	__blob_for_each_attr(pos, attr, len) {
		// check the name
		if (strcmp(currentName, blobmsg_name(pos)) == 0) {
			// check the type
			type 	= blobmsg_type(pos);
			//BLOBMSG_DBG_PRINT("item:: name: %s, type: %d\n", blobmsg_name(pos), type );

			// descend into arrays and tables
			if 	(	type == BLOBMSG_TYPE_TABLE ||
					type == BLOBMSG_TYPE_ARRAY
				)
			{
				ret = _searchTraverse(blobmsg_data(pos), blobmsg_data_len(pos), NULL);

				// not descending further
				if (ret == NULL) {
					ret = pos;
				}

				return ret;
			}
			else {
				return pos;
			}
		} // if names match
	} // blob for each attr

	return NULL;
}

int blobAttrValToChar(struct blob_attr *attr, char* ret, int *type)
{
	// check for valid blob_attr
	if (attr == NULL) {
		sprintf(ret, "Not found");
		*type = BLOBMSG_TYPE_UNSPEC;
		return EXIT_FAILURE;
	}

	switch ( blobmsg_type(attr) ) {
		case BLOBMSG_TYPE_ARRAY:
			sprintf(ret, "array");
			*type = BLOBMSG_TYPE_ARRAY;
			break;
		case BLOBMSG_TYPE_TABLE:
			sprintf(ret, "table");
			*type = BLOBMSG_TYPE_TABLE;
			break;

		case BLOBMSG_TYPE_STRING:
			sprintf(ret, "%s", blobmsg_get_string(attr) );
			*type = BLOBMSG_TYPE_STRING;
			break;

		case BLOBMSG_TYPE_INT8:
			sprintf(ret, "%d", (int)blobmsg_get_u8(attr) );
			*type = BLOBMSG_TYPE_INT8;
			break;
		case BLOBMSG_TYPE_INT16:
			sprintf(ret, "%d", (int)blobmsg_get_u16(attr) );
			*type = BLOBMSG_TYPE_INT16;
			break;
		case BLOBMSG_TYPE_INT32:
			sprintf(ret, "%d", (int)blobmsg_get_u32(attr) );
			*type = BLOBMSG_TYPE_INT32;
			break;
		case BLOBMSG_TYPE_INT64:
			sprintf(ret, "%d", (int)blobmsg_get_u64(attr) );
			*type = BLOBMSG_TYPE_INT64;
			break;

		case BLOBMSG_TYPE_UNSPEC:
		default:
			sprintf(ret, "unknown");
			*type = BLOBMSG_TYPE_UNSPEC;
			break;
	}

	return EXIT_SUCCESS;
}