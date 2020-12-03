/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/log.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv6-address.h"
#include "ns3/address-utils.h"
#include "ns3/nstime.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/socket.h"
#include "ns3/udp-socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "video-stream-client.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("VideoStreamClientApplication");

NS_OBJECT_ENSURE_REGISTERED (VideoStreamClient);

TypeId
VideoStreamClient::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::VideoStreamClient")
    .SetParent<Application> ()
    .SetGroupName ("Applications")
    .AddConstructor<VideoStreamClient> ()
    .AddAttribute ("Port", "Port on which we listen for incoming packets.",
                    UintegerValue (9),
                    MakeUintegerAccessor (&VideoStreamClient::m_port),
                    MakeUintegerChecker<uint16_t> ())
  ;
  return tid;
}

VideoStreamClient::VideoStreamClient ()
{
  NS_LOG_FUNCTION (this);
  m_initialDelay = 3;
  m_currentBufferSize = 0;
  m_frameRate = 25;
  m_videoLevel = 1;
  m_stopCounter = 0;
  m_rebufferCounter = 0;
  m_bufferEvent = EventId();
  m_sendEvent = EventId();
}

VideoStreamClient::~VideoStreamClient ()
{
  NS_LOG_FUNCTION (this);
  m_socket = 0;
}

void
VideoStreamClient::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  Application::DoDispose ();
}

void 
VideoStreamClient::StartApplication (void)
{
  NS_LOG_FUNCTION (this);

  if (m_socket == 0)
  {
    TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
    m_socket = Socket::CreateSocket (GetNode (), tid);
    InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), m_port);
    if (m_socket->Bind (local) == -1)
    {
      NS_FATAL_ERROR ("Failed to bind socket");
    }
    if (addressUtils::IsMulticast (m_local))
    {
      Ptr<UdpSocket> udpSocket = DynamicCast<UdpSocket> (m_socket);
      if (udpSocket)
      {
        udpSocket->MulticastJoinGroup (0, m_local);
      }
      else
      {
        NS_FATAL_ERROR ("Error: Failed to join multicast group");
      }
    }
  }

  m_socket->SetRecvCallback (MakeCallback (&VideoStreamClient::HandleRead, this));

  m_bufferEvent = Simulator::Schedule (Seconds (m_initialDelay), &VideoStreamClient::ReadFromBuffer, this);
}

void
VideoStreamClient::StopApplication ()
{
  NS_LOG_FUNCTION (this);

  if (m_socket != 0)
  {
    m_socket->Close ();
    m_socket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket>> ());
    m_socket = 0;
  }

  Simulator::Cancel (m_bufferEvent);
}

uint32_t 
VideoStreamClient::ReadFromBuffer (void)
{
  if (m_currentBufferSize == 0)
  {
    // NS_LOG_INFO ("The video frame buffer is empty!");

    m_stopCounter++;
    if (m_stopCounter < 3)
    {
      m_bufferEvent = Simulator::Schedule (Seconds (1.0), &VideoStreamClient::ReadFromBuffer, this);
    }
    // If the counter reaches 3, which means the client has been waiting for 3 sec, and no packets arrived.
    // In this case, we think the video streaming has finished, and there is no need to schedule the event.
    return (-1);
  }
  else if (m_currentBufferSize < m_frameRate) 
  {
    // NS_LOG_INFO ("Not enough frames in the buffer, rebuffering!");

    if (m_stopCounter > 0) m_stopCounter = 0;    // reset the stopCounter
    m_rebufferCounter++;
    m_bufferEvent = Simulator::Schedule (Seconds (1.0), &VideoStreamClient::ReadFromBuffer, this);
    return (-1);
  }
  else
  {
    // NS_LOG_INFO ("Play video frames from the buffer");
    if (m_stopCounter > 0) m_stopCounter = 0;    // reset the stopCounter
    if (m_rebufferCounter > 0) m_rebufferCounter = 0;   // reset the rebufferCounter
    m_currentBufferSize -= m_frameRate;

    m_bufferEvent = Simulator::Schedule (Seconds (1.0), &VideoStreamClient::ReadFromBuffer, this);
    return (m_currentBufferSize);
  }
}

void 
VideoStreamClient::HandleRead (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);

  Ptr<Packet> packet;
  Address from;
  Address localAddress;
  while ((packet = socket->RecvFrom (from)))
  {
    socket->GetSockName (localAddress);
    if (InetSocketAddress::IsMatchingType (from))
    {
      // NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s client received " << packet->GetSize () << " bytes from " << InetSocketAddress::ConvertFrom (from).GetIpv4 () << " port " << InetSocketAddress::ConvertFrom (from).GetPort ());

      m_currentBufferSize++;
      uint8_t dataBuffer[10];

      // The rebuffering event has happend 3+ times, which suggest the client to lower the video quality.
      if (m_rebufferCounter >= 3)
      {
        if (m_videoLevel > 1)
        {
          NS_LOG_INFO (Simulator::Now ().GetSeconds () << "s: Lower the video quality level!");
          m_videoLevel--;
          // reflect the change to the server
          sprintf((char *) dataBuffer, "%hu", m_videoLevel);
          Ptr<Packet> levelPacket = Create<Packet> (dataBuffer, 10);
          socket->SendTo (levelPacket, 0, from);
        }
      }

      
      // If the current buffer size supports 5+ seconds video, we can try to increase the video quality level.
      if (m_currentBufferSize > 5 * m_frameRate)
      {
        if (m_videoLevel < MAX_VIDEO_LEVEL)
        {
          NS_LOG_INFO (Simulator::Now ().GetSeconds() << "s: Increase the video quality level!");
          m_videoLevel++;
          // reflect the change to the server
          sprintf((char *) dataBuffer, "%hu", m_videoLevel);
          Ptr<Packet> levelPacket = Create<Packet> (dataBuffer, 10);
          socket->SendTo (levelPacket, 0, from);
        }
      }
    }
  }
}

}