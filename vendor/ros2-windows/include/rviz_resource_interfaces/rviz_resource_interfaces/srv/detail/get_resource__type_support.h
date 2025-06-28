// generated from rosidl_generator_c/resource/idl__type_support.h.em
// with input from rviz_resource_interfaces:srv\GetResource.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "rviz_resource_interfaces/srv/get_resource.h"


#ifndef RVIZ_RESOURCE_INTERFACES__SRV__DETAIL__GET_RESOURCE__TYPE_SUPPORT_H_
#define RVIZ_RESOURCE_INTERFACES__SRV__DETAIL__GET_RESOURCE__TYPE_SUPPORT_H_

#include "rosidl_typesupport_interface/macros.h"

#include "rviz_resource_interfaces/msg/rosidl_generator_c__visibility_control.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include "rosidl_runtime_c/message_type_support_struct.h"

// Forward declare the get type support functions for this type.
ROSIDL_GENERATOR_C_PUBLIC_rviz_resource_interfaces
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(
  rosidl_typesupport_c,
  rviz_resource_interfaces,
  srv,
  GetResource_Request
)(void);

// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"

// Forward declare the get type support functions for this type.
ROSIDL_GENERATOR_C_PUBLIC_rviz_resource_interfaces
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(
  rosidl_typesupport_c,
  rviz_resource_interfaces,
  srv,
  GetResource_Response
)(void);

// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"

// Forward declare the get type support functions for this type.
ROSIDL_GENERATOR_C_PUBLIC_rviz_resource_interfaces
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(
  rosidl_typesupport_c,
  rviz_resource_interfaces,
  srv,
  GetResource_Event
)(void);

#include "rosidl_runtime_c/service_type_support_struct.h"

// Forward declare the get type support functions for this type.
ROSIDL_GENERATOR_C_PUBLIC_rviz_resource_interfaces
const rosidl_service_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(
  rosidl_typesupport_c,
  rviz_resource_interfaces,
  srv,
  GetResource
)(void);

// Forward declare the function to create a service event message for this type.
ROSIDL_GENERATOR_C_PUBLIC_rviz_resource_interfaces
void *
ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_CREATE_EVENT_MESSAGE_SYMBOL_NAME(
  rosidl_typesupport_c,
  rviz_resource_interfaces,
  srv,
  GetResource
)(
  const rosidl_service_introspection_info_t * info,
  rcutils_allocator_t * allocator,
  const void * request_message,
  const void * response_message);

// Forward declare the function to destroy a service event message for this type.
ROSIDL_GENERATOR_C_PUBLIC_rviz_resource_interfaces
bool
ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_DESTROY_EVENT_MESSAGE_SYMBOL_NAME(
  rosidl_typesupport_c,
  rviz_resource_interfaces,
  srv,
  GetResource
)(
  void * event_msg,
  rcutils_allocator_t * allocator);

#ifdef __cplusplus
}
#endif

#endif  // RVIZ_RESOURCE_INTERFACES__SRV__DETAIL__GET_RESOURCE__TYPE_SUPPORT_H_
