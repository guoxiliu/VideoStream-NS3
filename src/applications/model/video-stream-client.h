/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef VIDEO_STREAM_CLIENT_H
#define VIDEO_STREAM_CLIENT_H

#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/address.h"
#include "ns3/traced-callback.h"

namespace ns3 {

class Socket;
class Packet;

/**
 * @brief A Video Stream Client
 */
class VideoStreamClient : public Application
{
public:
/**
 * @brief Get the type ID.
 * 
 * @return the object TypeId
 */
  static TypeId GetTypeId (void);
  VideoStreamClient ();
  virtual ~VideoStreamClient ();

  /**
   * @brief Read data from the frame buffer. If the buffer does not have 
   * enough frames, it will reschedule the reading event next second.
   * 
   * @return the updated buffer size (-1 if the buffer size is smaller than the fps)
   */
  uint32_t ReadFromBuffer (void);

protected:
  virtual void DoDispose (void);

private: 
  virtual void StartApplication (void);
  virtual void StopApplication (void);

  /**
   * @brief Handle a packet reception.
   * 
   * This function is called by lower layers.
   * 
   * @param socket the socket the packet was received to
   */
  void HandleRead (Ptr<Socket> socket);

  uint16_t m_port; //!< Remote port
  Ptr<Socket> m_socket; //!< Socket
  Address m_local; //!< Local multicast address

  uint16_t m_initialDelay; //!< Seconds to wait before displaying the content
  uint32_t m_frameRate; //!< Number of frames per second to be played
  uint32_t m_currentBufferSize; //!< Size of the frame buffer
  EventId m_bufferEvent; //!< Event to read from the buffer

};

} // namespace ns3

#endif /* VIDEO_STREAM_CLIENT_H */