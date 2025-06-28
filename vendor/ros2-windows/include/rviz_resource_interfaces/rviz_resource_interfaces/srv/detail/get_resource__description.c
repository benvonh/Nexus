// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from rviz_resource_interfaces:srv\GetResource.idl
// generated code does not contain a copyright notice

#include "rviz_resource_interfaces/srv/detail/get_resource__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_rviz_resource_interfaces
const rosidl_type_hash_t *
rviz_resource_interfaces__srv__GetResource__get_type_hash(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xf2, 0x16, 0x66, 0xbc, 0xa3, 0xa1, 0x35, 0x4c,
      0xd3, 0x5a, 0x1d, 0x74, 0x7d, 0x5c, 0x66, 0xa6,
      0x99, 0xa6, 0xa9, 0xce, 0x79, 0x72, 0x95, 0x50,
      0x10, 0x6d, 0x00, 0x82, 0x58, 0x35, 0xa3, 0xa6,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_rviz_resource_interfaces
const rosidl_type_hash_t *
rviz_resource_interfaces__srv__GetResource_Request__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x8e, 0x60, 0x83, 0x8c, 0x0f, 0xb8, 0xfb, 0x33,
      0xbc, 0x06, 0x56, 0x20, 0x3f, 0x00, 0x18, 0x13,
      0x0a, 0xd1, 0x5a, 0x42, 0xe4, 0x8b, 0x5c, 0x3e,
      0xef, 0x4a, 0x25, 0x7a, 0x88, 0xb8, 0x46, 0x0d,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_rviz_resource_interfaces
const rosidl_type_hash_t *
rviz_resource_interfaces__srv__GetResource_Response__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x0f, 0x0c, 0xdf, 0x27, 0x99, 0xe3, 0x83, 0xc6,
      0x4e, 0x47, 0xac, 0x31, 0xae, 0x65, 0xe0, 0x96,
      0xb5, 0xe8, 0x23, 0xff, 0xc6, 0x2c, 0xe8, 0x14,
      0x8f, 0x83, 0x83, 0x73, 0xe1, 0xc5, 0x0d, 0x23,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_rviz_resource_interfaces
const rosidl_type_hash_t *
rviz_resource_interfaces__srv__GetResource_Event__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x74, 0x4d, 0x52, 0xf0, 0x6c, 0xd0, 0x05, 0x16,
      0xb5, 0x55, 0x02, 0x70, 0x87, 0x6d, 0xa0, 0xcb,
      0x91, 0xd8, 0x5d, 0x92, 0xbc, 0xba, 0x2d, 0x90,
      0xfe, 0x1f, 0xde, 0xfb, 0x1b, 0xe5, 0x89, 0x66,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "builtin_interfaces/msg/detail/time__functions.h"
#include "service_msgs/msg/detail/service_event_info__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t builtin_interfaces__msg__Time__EXPECTED_HASH = {1, {
    0xb1, 0x06, 0x23, 0x5e, 0x25, 0xa4, 0xc5, 0xed,
    0x35, 0x09, 0x8a, 0xa0, 0xa6, 0x1a, 0x3e, 0xe9,
    0xc9, 0xb1, 0x8d, 0x19, 0x7f, 0x39, 0x8b, 0x0e,
    0x42, 0x06, 0xce, 0xa9, 0xac, 0xf9, 0xc1, 0x97,
  }};
static const rosidl_type_hash_t service_msgs__msg__ServiceEventInfo__EXPECTED_HASH = {1, {
    0x41, 0xbc, 0xbb, 0xe0, 0x7a, 0x75, 0xc9, 0xb5,
    0x2b, 0xc9, 0x6b, 0xfd, 0x5c, 0x24, 0xd7, 0xf0,
    0xfc, 0x0a, 0x08, 0xc0, 0xcb, 0x79, 0x21, 0xb3,
    0x37, 0x3c, 0x57, 0x32, 0x34, 0x5a, 0x6f, 0x45,
  }};
#endif

static char rviz_resource_interfaces__srv__GetResource__TYPE_NAME[] = "rviz_resource_interfaces/srv/GetResource";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";
static char rviz_resource_interfaces__srv__GetResource_Event__TYPE_NAME[] = "rviz_resource_interfaces/srv/GetResource_Event";
static char rviz_resource_interfaces__srv__GetResource_Request__TYPE_NAME[] = "rviz_resource_interfaces/srv/GetResource_Request";
static char rviz_resource_interfaces__srv__GetResource_Response__TYPE_NAME[] = "rviz_resource_interfaces/srv/GetResource_Response";
static char service_msgs__msg__ServiceEventInfo__TYPE_NAME[] = "service_msgs/msg/ServiceEventInfo";

// Define type names, field names, and default values
static char rviz_resource_interfaces__srv__GetResource__FIELD_NAME__request_message[] = "request_message";
static char rviz_resource_interfaces__srv__GetResource__FIELD_NAME__response_message[] = "response_message";
static char rviz_resource_interfaces__srv__GetResource__FIELD_NAME__event_message[] = "event_message";

static rosidl_runtime_c__type_description__Field rviz_resource_interfaces__srv__GetResource__FIELDS[] = {
  {
    {rviz_resource_interfaces__srv__GetResource__FIELD_NAME__request_message, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {rviz_resource_interfaces__srv__GetResource_Request__TYPE_NAME, 48, 48},
    },
    {NULL, 0, 0},
  },
  {
    {rviz_resource_interfaces__srv__GetResource__FIELD_NAME__response_message, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {rviz_resource_interfaces__srv__GetResource_Response__TYPE_NAME, 49, 49},
    },
    {NULL, 0, 0},
  },
  {
    {rviz_resource_interfaces__srv__GetResource__FIELD_NAME__event_message, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {rviz_resource_interfaces__srv__GetResource_Event__TYPE_NAME, 46, 46},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription rviz_resource_interfaces__srv__GetResource__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {rviz_resource_interfaces__srv__GetResource_Event__TYPE_NAME, 46, 46},
    {NULL, 0, 0},
  },
  {
    {rviz_resource_interfaces__srv__GetResource_Request__TYPE_NAME, 48, 48},
    {NULL, 0, 0},
  },
  {
    {rviz_resource_interfaces__srv__GetResource_Response__TYPE_NAME, 49, 49},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
rviz_resource_interfaces__srv__GetResource__get_type_description(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {rviz_resource_interfaces__srv__GetResource__TYPE_NAME, 40, 40},
      {rviz_resource_interfaces__srv__GetResource__FIELDS, 3, 3},
    },
    {rviz_resource_interfaces__srv__GetResource__REFERENCED_TYPE_DESCRIPTIONS, 5, 5},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[1].fields = rviz_resource_interfaces__srv__GetResource_Event__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[2].fields = rviz_resource_interfaces__srv__GetResource_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[3].fields = rviz_resource_interfaces__srv__GetResource_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[4].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char rviz_resource_interfaces__srv__GetResource_Request__FIELD_NAME__path[] = "path";
static char rviz_resource_interfaces__srv__GetResource_Request__FIELD_NAME__etag[] = "etag";

static rosidl_runtime_c__type_description__Field rviz_resource_interfaces__srv__GetResource_Request__FIELDS[] = {
  {
    {rviz_resource_interfaces__srv__GetResource_Request__FIELD_NAME__path, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {rviz_resource_interfaces__srv__GetResource_Request__FIELD_NAME__etag, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
rviz_resource_interfaces__srv__GetResource_Request__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {rviz_resource_interfaces__srv__GetResource_Request__TYPE_NAME, 48, 48},
      {rviz_resource_interfaces__srv__GetResource_Request__FIELDS, 2, 2},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char rviz_resource_interfaces__srv__GetResource_Response__FIELD_NAME__status_code[] = "status_code";
static char rviz_resource_interfaces__srv__GetResource_Response__FIELD_NAME__error_reason[] = "error_reason";
static char rviz_resource_interfaces__srv__GetResource_Response__FIELD_NAME__expanded_path[] = "expanded_path";
static char rviz_resource_interfaces__srv__GetResource_Response__FIELD_NAME__etag[] = "etag";
static char rviz_resource_interfaces__srv__GetResource_Response__FIELD_NAME__body[] = "body";

static rosidl_runtime_c__type_description__Field rviz_resource_interfaces__srv__GetResource_Response__FIELDS[] = {
  {
    {rviz_resource_interfaces__srv__GetResource_Response__FIELD_NAME__status_code, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {rviz_resource_interfaces__srv__GetResource_Response__FIELD_NAME__error_reason, 12, 12},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {rviz_resource_interfaces__srv__GetResource_Response__FIELD_NAME__expanded_path, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {rviz_resource_interfaces__srv__GetResource_Response__FIELD_NAME__etag, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {rviz_resource_interfaces__srv__GetResource_Response__FIELD_NAME__body, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8_UNBOUNDED_SEQUENCE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
rviz_resource_interfaces__srv__GetResource_Response__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {rviz_resource_interfaces__srv__GetResource_Response__TYPE_NAME, 49, 49},
      {rviz_resource_interfaces__srv__GetResource_Response__FIELDS, 5, 5},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char rviz_resource_interfaces__srv__GetResource_Event__FIELD_NAME__info[] = "info";
static char rviz_resource_interfaces__srv__GetResource_Event__FIELD_NAME__request[] = "request";
static char rviz_resource_interfaces__srv__GetResource_Event__FIELD_NAME__response[] = "response";

static rosidl_runtime_c__type_description__Field rviz_resource_interfaces__srv__GetResource_Event__FIELDS[] = {
  {
    {rviz_resource_interfaces__srv__GetResource_Event__FIELD_NAME__info, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    },
    {NULL, 0, 0},
  },
  {
    {rviz_resource_interfaces__srv__GetResource_Event__FIELD_NAME__request, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {rviz_resource_interfaces__srv__GetResource_Request__TYPE_NAME, 48, 48},
    },
    {NULL, 0, 0},
  },
  {
    {rviz_resource_interfaces__srv__GetResource_Event__FIELD_NAME__response, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {rviz_resource_interfaces__srv__GetResource_Response__TYPE_NAME, 49, 49},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription rviz_resource_interfaces__srv__GetResource_Event__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {rviz_resource_interfaces__srv__GetResource_Request__TYPE_NAME, 48, 48},
    {NULL, 0, 0},
  },
  {
    {rviz_resource_interfaces__srv__GetResource_Response__TYPE_NAME, 49, 49},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
rviz_resource_interfaces__srv__GetResource_Event__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {rviz_resource_interfaces__srv__GetResource_Event__TYPE_NAME, 46, 46},
      {rviz_resource_interfaces__srv__GetResource_Event__FIELDS, 3, 3},
    },
    {rviz_resource_interfaces__srv__GetResource_Event__REFERENCED_TYPE_DESCRIPTIONS, 4, 4},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[1].fields = rviz_resource_interfaces__srv__GetResource_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[2].fields = rviz_resource_interfaces__srv__GetResource_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[3].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# Resource path of the requested resource.\n"
  "string path\n"
  "\n"
  "# HTTP-style ETag value for the requested resource.\n"
  "# See: https://en.wikipedia.org/wiki/HTTP_ETag\n"
  "#\n"
  "# If this value is empty, then the server shall respond with the current\n"
  "# version of the resource and the ETag value, if it can be loaded.\n"
  "# If this value is not-empty, then the server may respond with a new resource\n"
  "# and ETag value.\n"
  "# However, if the ETag value for the resource has not changed, then the server\n"
  "# may respond with NOT_MODIFIED as the status_code, similar to\n"
  "# \"HTTP 304: Not Modified\".\n"
  "# See: https://en.wikipedia.org/wiki/List_of_HTTP_status_codes#304\n"
  "# The server may also ignore this value and always send the current version\n"
  "# of the resource and its ETag value, if caching is not implemented.\n"
  "string etag\n"
  "\n"
  "---\n"
  "\n"
  "# An unspecified error occurred, check the error_reason string.\n"
  "int32 ERROR=0\n"
  "\n"
  "# The request was successful, etag and body will be set with valid values,\n"
  "# though etag may be empty.\n"
  "# The error_reason will be empty.\n"
  "int32 OK=1\n"
  "\n"
  "# The request was successful, but the etag value has not changed.\n"
  "# The etag value will be set to the requested etag value, but the body value\n"
  "# will be empty.\n"
  "# The error_reason should also be empty.\n"
  "int32 NOT_MODIFIED=2\n"
  "\n"
  "# Status code for the request, can be one of the above options.\n"
  "int32 status_code\n"
  "\n"
  "# Optionally set error reason string.\n"
  "string error_reason\n"
  "\n"
  "# Expanded path, which may or may not be different from the given path.\n"
  "# The Service may expand, extend, or otherwise further qualify the path as it\n"
  "# resolves it, any of which would be reflected in this expanded path.\n"
  "string expanded_path\n"
  "\n"
  "# HTTP-style ETag value for the requested resource.\n"
  "# See: https://en.wikipedia.org/wiki/HTTP_ETag\n"
  "#\n"
  "# As with the HTTP ETag, the value is unspecified, but it is described as:\n"
  "#\n"
  "# > Common methods of ETag generation include using a collision-resistant hash\n"
  "# > function of the resource's content, a hash of the last modification\n"
  "# > timestamp, or even just a revision number.\n"
  "#\n"
  "# This value may be empty if the server does not implement cache checking.\n"
  "#\n"
  "# This can be sent on subsequent requests to avoid getting the same unchanged\n"
  "# resource multiple times.\n"
  "string etag\n"
  "\n"
  "# Opaque value of the resource.\n"
  "uint8[] body";

static char srv_encoding[] = "srv";
static char implicit_encoding[] = "implicit";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
rviz_resource_interfaces__srv__GetResource__get_individual_type_description_source(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {rviz_resource_interfaces__srv__GetResource__TYPE_NAME, 40, 40},
    {srv_encoding, 3, 3},
    {toplevel_type_raw_source, 2246, 2246},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
rviz_resource_interfaces__srv__GetResource_Request__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {rviz_resource_interfaces__srv__GetResource_Request__TYPE_NAME, 48, 48},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
rviz_resource_interfaces__srv__GetResource_Response__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {rviz_resource_interfaces__srv__GetResource_Response__TYPE_NAME, 49, 49},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
rviz_resource_interfaces__srv__GetResource_Event__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {rviz_resource_interfaces__srv__GetResource_Event__TYPE_NAME, 46, 46},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
rviz_resource_interfaces__srv__GetResource__get_type_description_sources(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[6];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 6, 6};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *rviz_resource_interfaces__srv__GetResource__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *rviz_resource_interfaces__srv__GetResource_Event__get_individual_type_description_source(NULL);
    sources[3] = *rviz_resource_interfaces__srv__GetResource_Request__get_individual_type_description_source(NULL);
    sources[4] = *rviz_resource_interfaces__srv__GetResource_Response__get_individual_type_description_source(NULL);
    sources[5] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
rviz_resource_interfaces__srv__GetResource_Request__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *rviz_resource_interfaces__srv__GetResource_Request__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
rviz_resource_interfaces__srv__GetResource_Response__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *rviz_resource_interfaces__srv__GetResource_Response__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
rviz_resource_interfaces__srv__GetResource_Event__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[5];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 5, 5};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *rviz_resource_interfaces__srv__GetResource_Event__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *rviz_resource_interfaces__srv__GetResource_Request__get_individual_type_description_source(NULL);
    sources[3] = *rviz_resource_interfaces__srv__GetResource_Response__get_individual_type_description_source(NULL);
    sources[4] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
