/* Copyright 2019 Google LLC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <iotc_bsp_io_net.h>
#include <iotc_bsp_debug.h>

#include <fcntl.h>
#ifdef CONFIG_POSIX_API
#include <unistd.h>
#include <sys/socket.h>
#include <poll.h>
#include <netdb.h>
#else
#include <net/socket.h>
#endif
#include <stdio.h>

#include "iotc_macros.h"
#include <errno.h>

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

iotc_bsp_io_net_state_t iotc_bsp_io_net_socket_connect(
    iotc_bsp_socket_t* iotc_socket, const char* host, uint16_t port,
    iotc_bsp_socket_type_t socket_type) {
  struct addrinfo hints;
  struct addrinfo *result, *rp = NULL;
  int status;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = socket_type;
  hints.ai_flags = 0;
  hints.ai_protocol = 0;

  // Address resolution.
  status = getaddrinfo(host, NULL, &hints, &result);
  if (0 != status) {
    return IOTC_BSP_IO_NET_STATE_ERROR;
  }

  for (rp = result; rp != NULL; rp = rp->ai_next) {
    *iotc_socket = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if (-1 == *iotc_socket) continue;

    // Set the socket to be non-blocking
    const int flags = fcntl(*iotc_socket, F_GETFL, 0);
    if (-1 == fcntl(*iotc_socket, F_SETFL, flags | O_NONBLOCK)) {
      freeaddrinfo(result);
      return IOTC_BSP_IO_NET_STATE_ERROR;
    }

    switch (rp->ai_family) {
      case AF_INET6:
        ((struct sockaddr_in6*)(rp->ai_addr))->sin6_port = htons(port);
        break;
      case AF_INET:
        ((struct sockaddr_in*)(rp->ai_addr))->sin_port = htons(port);
        break;
      default:
        return IOTC_BSP_IO_NET_STATE_ERROR;
        break;
    }

    // Attempt to connect.
    status = connect(*iotc_socket, rp->ai_addr, rp->ai_addrlen);

    if (-1 != status) {
      freeaddrinfo(result);
      return IOTC_BSP_IO_NET_STATE_OK;
    } else {
      if (EINPROGRESS == errno) {
        freeaddrinfo(result);
        return IOTC_BSP_IO_NET_STATE_OK;
      } else {
        close(*iotc_socket);
      }
    }
  }
  freeaddrinfo(result);
  return IOTC_BSP_IO_NET_STATE_ERROR;
}

iotc_bsp_io_net_state_t iotc_bsp_io_net_connection_check(
    iotc_bsp_socket_t iotc_socket, const char* host, uint16_t port) {
  IOTC_UNUSED(host);
  IOTC_UNUSED(port);

  return IOTC_BSP_IO_NET_STATE_OK;
}

iotc_bsp_io_net_state_t iotc_bsp_io_net_write(iotc_bsp_socket_t iotc_socket,
                                              int* out_written_count,
                                              const uint8_t* buf,
                                              size_t count) {
  if (NULL == out_written_count || NULL == buf) {
    return IOTC_BSP_IO_NET_STATE_ERROR;
  }

  int errval = 0;
  *out_written_count = send(iotc_socket, buf, count, 0);

  if (*out_written_count < 0) {
    *out_written_count = 0;

    errval = errno;

    if (EAGAIN == errval) {
      return IOTC_BSP_IO_NET_STATE_BUSY;
    }

    if (ECONNRESET == errval || EPIPE == errval) {
      return IOTC_BSP_IO_NET_STATE_CONNECTION_RESET;
    }

    return IOTC_BSP_IO_NET_STATE_ERROR;
  }

  return IOTC_BSP_IO_NET_STATE_OK;
}

iotc_bsp_io_net_state_t iotc_bsp_io_net_read(iotc_bsp_socket_t iotc_socket,
                                             int* out_read_count, uint8_t* buf,
                                             size_t count) {
  if (NULL == out_read_count || NULL == buf) {
    return IOTC_BSP_IO_NET_STATE_ERROR;
  }

  int errval = 0;
  *out_read_count = recv(iotc_socket, buf, count, MSG_DONTWAIT);

  if (*out_read_count < 0) {
    *out_read_count = 0;

    errval = errno;

    if (EAGAIN == errval) {
      return IOTC_BSP_IO_NET_STATE_BUSY;
    }

    if (ECONNRESET == errval || EPIPE == errval) {
      return IOTC_BSP_IO_NET_STATE_CONNECTION_RESET;
    }

    return IOTC_BSP_IO_NET_STATE_ERROR;
  }

  if (0 == *out_read_count) {
    return IOTC_BSP_IO_NET_STATE_CONNECTION_RESET;
  }

  return IOTC_BSP_IO_NET_STATE_OK;
}

iotc_bsp_io_net_state_t iotc_bsp_io_net_close_socket(
    iotc_bsp_socket_t* iotc_socket) {
  if (NULL == iotc_socket) {
    return IOTC_BSP_IO_NET_STATE_ERROR;
  }

  close(*iotc_socket);

  *iotc_socket = 0;

  return IOTC_BSP_IO_NET_STATE_OK;
}

#define FD_SET(socket, event, pollfd) \
  pollfd.fd = socket;                 \
  pollfd.events |= event;
#define FD_ISSET(event, pollfd) (pollfd.revents | event)

iotc_bsp_io_net_state_t iotc_bsp_io_net_select(
    iotc_bsp_socket_events_t* socket_events_array,
    size_t socket_events_array_size, long timeout_sec) {
  struct pollfd fds[1] = {0};  // note: single socket support

  /* currently, only one socket (connection) is supported */
  if (socket_events_array_size > 1) {
    iotc_bsp_debug_format("Error: only one connection is currently supported (%ld)\n",
        socket_events_array_size);
    return IOTC_BSP_IO_NET_STATE_ERROR;
  }

  /* translate the library socket events settings to the event sets used by
   * Zephyr poll mechanism
   * Note: currently only one socket is supported
   */
  size_t socket_id = 0;
  for (socket_id = 0; socket_id < socket_events_array_size; ++socket_id) {
    iotc_bsp_socket_events_t* socket_events = &socket_events_array[socket_id];

    if (NULL == socket_events) {
      return IOTC_BSP_IO_NET_STATE_ERROR;
    }

    if (1 == socket_events->in_socket_want_read) {
      FD_SET(socket_events->iotc_socket, POLLIN, fds[0]);
    }

    if ((1 == socket_events->in_socket_want_write) ||
        (1 == socket_events->in_socket_want_connect)) {
      FD_SET(socket_events->iotc_socket, POLLOUT, fds[0]);
    }

    if (1 == socket_events->in_socket_want_error) {
      FD_SET(socket_events->iotc_socket, POLLERR, fds[0]);
    }
  }

  /* call the actual posix select */
  const int result = poll(fds, socket_events_array_size, timeout_sec);

  if (socket_events_array_size == 0 || 0 < result) {
    /* translate the result back to the socket events structure */
    for (socket_id = 0; socket_id < socket_events_array_size; ++socket_id) {
      iotc_bsp_socket_events_t* socket_events = &socket_events_array[socket_id];

      if (FD_ISSET(POLLIN, fds[0])) {
        socket_events->out_socket_can_read = 1;
      }

      if (FD_ISSET(POLLOUT, fds[0])) {
        if (1 == socket_events->in_socket_want_connect) {
          socket_events->out_socket_connect_finished = 1;
        }

        if (1 == socket_events->in_socket_want_write) {
          socket_events->out_socket_can_write = 1;
        }
      }

      if (FD_ISSET(POLLERR, fds[0])) {
        socket_events->out_socket_error = 1;
      }
    }

    return IOTC_BSP_IO_NET_STATE_OK;
  } else if (0 == result) {
    return IOTC_BSP_IO_NET_STATE_TIMEOUT;
  }

  return IOTC_BSP_IO_NET_STATE_ERROR;
}

