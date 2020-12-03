/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef VIDEO_STREAM_SERVER_H
#define VIDEO_STREAM_SERVER_H

#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/string.h"
#include "ns3/ipv4-address.h"
#include "ns3/traced-callback.h"

#include <fstream>

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
     * @brief Set the name of the file containing the frame sizes.
     * 
      * @param frameFile the file name
     */
    void SetFrameFile (std::string frameFile);

    /**
     * @brief Get the name of the file containing the frame sizes.
     * 
     * @return the file name 
     */
    std::string GetFrameFile (void) const;

    /**
     * @brief Set the maximum packet size.
     * 
     * @param maxPacketSize the largest number of bytes a packet can be
     */
    void SetMaxPacketSize (uint32_t maxPacketSize);

    /**
     * @brief Get the maximum packet size.
     * 
     * @return uint32_t the largest number of bytes a packet can be
     */
    uint32_t GetMaxPacketSize (void) const;

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
     * @brief Send a packet with specified size.
     * 
     * @param packetSize the number of bytes for the packet to be sent
     */
    void SendPacket (uint32_t packetSize);
    
    /**
     * @brief Send the video frame from the frame list.
     */
    void Send (void);

    /**
     * @brief Handle a packet reception.
     * 
     * This function is called by lower layers.
     * 
     * @param socket the socket the packet was received to
     */
    void HandleRead (Ptr<Socket> socket);

    Time m_interval; //!< Packet inter-send time
    uint32_t m_maxPacketSize; //!< Maximum size of the packet to be sent

    uint32_t m_sent; //!< Counter for sent frames
    Ptr<Socket> m_socket; //!< Socket
    Address m_peerAddress; //!< Remote peer address
    uint16_t m_peerPort; //!< Remote peer port
    EventId m_sendEvent; //!< Event to send the next packet

    uint32_t m_frameRate; //!< Number of frames per second to be sent
    std::string m_frameFile; //!< Name of the file containing frame sizes
    std::vector<uint32_t> m_frameSizeList; //!< List of video frame sizes
  };

} // namespace ns3


#endif /* VIDEO_STREAM_SERVER_H */