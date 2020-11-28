/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef VIDEO_STREAM_SERVER_H
#define VIDEO_STREAM_SERVER_H

#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/ipv4-address.h"
#include "ns3/traced-callback.h"

namespace ns3 {

class Socket;
class Packet;

  /**
   * @brief A Video Stream Server
   */
  class VideoStreamServer : public Application
  {
  public:
    /**
     * @brief Get the type ID.
     * 
     * @return the object TypeId
     */
    static TypeId GetTypeId (void);

    VideoStreamServer ();

    virtual ~VideoStreamServer ();

    /**
     * @brief Set the destination address and port.
     * 
     * @param ip destination IP address
     * @param port destination port
     */
    void SetRemote (Address ip, uint16_t port);
    /**
     * @brief Set the destination address.
     * 
     * @param addr destination address
     */
    void SetRemote (Address addr);

    /**
     * @brief Set the frame size of the video.
     * 
      * @param frameSize the number of bytes of each frame
     */
    void SetFrameSize (uint32_t frameSize);

    /**
     * @brief Get the frame size of the video.
     * 
     * @return the number of bytes of each frame
     */
    uint32_t GetFrameSize (void) const;

    /**
     * @brief Set the maximum packet size.
     * 
     * @param maxPacketSize the largest number of bytes a packet can be
     */
    void SetMaxPacketSize (uint16_t maxPacketSize);

    /**
     * @brief Get the maximum packet size.
     * 
     * @return uint16_t the largest number of bytes a packet can be
     */
    uint16_t GetMaxPacketSize (void) const;

  protected:
    virtual void DoDispose (void);

  private:

    virtual void StartApplication (void);
    virtual void StopApplication (void);

    /**
     * @brief Schedule the next packet transmission.
     * 
     * @param dt time interval between packets
     */
    void ScheduleTransmit (Time dt);
    
    /**
     * @brief Send a packet.
     */
    void Send (void);

    Time m_interval; //!< Packet inter-send time
    uint16_t m_maxPacketSize; //!< Maximum size of the packet to be sent

    Ptr<Socket> m_socket; //!< Socket
    Address m_peerAddress; //!< Remote peer address
    uint16_t m_peerPort; //!< Remote peer port
    EventId m_sendEvent; //!< Event to send the next packet

    uint32_t m_frameSize; //!< Size of each frame
    uint32_t m_frameRate; //!< Number of frames per second to be sent
  };

} // namespace ns3


#endif /* VIDEO_STREAM_SERVER_H */