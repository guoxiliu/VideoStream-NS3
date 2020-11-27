/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef VIDEO_STREAM_CLIENT_H
#define VIDEO_STREAM_CLIENT_H

#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/ipv4-address.h"
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
     * @brief Get the type ID
     * 
     * @return the object TypeId
     */
    static TypeId GetTypeId (void);

    VideoStreamClient ();

    virtual ~VideoStreamClient ();

    /**
     * @brief Set the remote address and port
     * 
     * @param ip remote IP address
     * @param port remote port
     */
    void SetRemote (Address ip, uint16_t port);
    /**
     * @brief Set the remote address
     * 
     * @param addr remote address
     */
    void SetRemote (Address addr);

    /**
     * @brief Set the data size of the packet
     * 
     * @param dataSize  the size of the data you want to send
     */
    void SetDataSize (uint32_t dataSize);

    /**
     * @brief Get the number of bytes that will be sent to the server.
     * 
     * @return the 
     */
    uint32_t GetDataSize (void) const;

  protected:
    virtual void DoDispose (void);

  private:

    virtual void StartApplication (void);
    virtual void StopApplication (void);

    /**
     * @brief Schedule the next packet transmission
     * 
     * @param dt time interval between packets
     */
    void ScheduleTransmit (Time dt);
    
    /**
     * @brief Send a packet
     */
    void Send (void);

    Time m_interval; //!< Packet inter-send time
    uint32_t m_size; //!< Size of the sent packet

    Ptr<Socket> m_socket; //!< Socket
    Address m_peerAddress; //!< Remote peer address
    uint16_t m_peerPort; //!< Remote peer port
    EventId m_sendEvent; //!< Event to send the next packet

    uint32_t m_frameRate; //!< Frame rate of the video
  };

} // namespace ns3


#endif /* VIDEO_STREAM_CLIENT_H */