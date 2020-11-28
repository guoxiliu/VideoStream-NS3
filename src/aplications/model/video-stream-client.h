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
   * @param socket the socket the packet was received to.
   */
  void HandleRead (Ptr<Socket> socket);

  uint16_t m_port;
  Ptr<Socket> m_socket;
  Ptr<Socket> m_socket6;
  Address m_local;

  uint32_t m_frameRate;
  EventId m_bufferEvent;

};

} // namespace ns3

#endif /* VIDEO_STREAM_SERVER_H */
