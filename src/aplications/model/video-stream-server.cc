/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/log.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv6-address.h"
#include "ns3/nstime.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/video-stream-server.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("VideoStreamServerApplication");

NS_OBJECT_ENSURE_REGISTERED (VideoStreamServer);

TypeId
VideoStreamServer::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::VideoStreamServer")
    .SetParent<Application> ()
    .SetGroupName("Applications")
    .AddConstructor<VideoStreamServer> ()
    .AddAttribute ("Interval",
                    "The time to wait between packets",
                    TimeValue (Seconds (1.0)),
                    MakeTimeAccessor (&VideoStreamServer::m_interval),
                    MakeTimeChecker ())
    .AddAttribute ("RemoteAddress",
                    "The destination address of the outbound packets",
                    AddressValue (),
                    MakeAddressAccessor (&VideoStreamServer::m_peerAddress),
                    MakeAddressChecker ())
    .AddAttribute ("RemotePort",
                    "The destination port of the outbound packets",
                    UintegerValue (0),
                    MakeUintegerAccessor (&VideoStreamServer::m_peerPort),
                    MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("PacketSize",
                    "Size of the outbound packets",
                    UintegerValue (100),
                    MakeUintegerAccessor (&VideoStreamServer::SetDataSize, &VideoStreamServer::GetDataSize),
                    MakeUintegerChecker<uint32_t> ())

    ;
    return tid;
}

VideoStreamServer::VideoStreamServer ()
{
  NS_LOG_FUNCTION (this);
  m_socket = 0;
  m_size = 0;
  m_frameRate = 0;
  m_sendEvent = EventId ();
}

VideoStreamServer::~VideoStreamServer ()
{
  NS_LOG_FUNCTION (this);
  m_socket = 0;
}

void
VideoStreamServer::SetRemote (Address ip, uint16_t port)
{
  NS_LOG_FUNCTION (this << ip << port);
  m_peerAddress = ip;
  m_peerPort = port;
}

void 
VideoStreamServer::SetRemote (Address addr)
{
  NS_LOG_FUNCTION (this << addr);
  m_peerAddress = addr;
}

void 
VideoStreamServer::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  Application::DoDispose ();
}

void
VideoStreamServer::StartApplication (void)
{
  NS_LOG_FUNCTION (this);

  if (m_socket == 0)
  {
    TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
    m_socket = Socket::CreateSocket (GetNode (), tid);
    if (Ipv4Address::IsMatchingType (m_peerAddress) == true)
    {
      if (m_socket->Bind () == -1)
      {
        NS_FATAL_ERROR ("Failed to bind socket");
      }
      m_socket->Connect (InetSocketAddress(Ipv4Address::ConvertFrom (m_peerAddress), m_peerPort));
    }
    else if (Ipv6Address::IsMatchingType (m_peerAddress) == true)
    {
      if (m_socket->Bind6 () == -1)
      {
        NS_FATAL_ERROR ("Failed to bind socket");
      }
      m_socket->Connect (m_peerAddress);
    }
    else if (InetSocketAddress::IsMatchingType (m_peerAddress) == true)
    {
      if (m_socket->Bind () == -1)
      {
        NS_FATAL_ERROR ("Failed to bind socket");
      }
      m_socket->Connect (m_peerAddress);
    }
    else if (Inet6SocketAddress::IsMatchingType (m_peerAddress) == true)
    {
      if (m_socket->Bind6 () == -1)
      {
        NS_FATAL_ERROR ("Failed to bind socket");
      }
      m_socket->Connect (m_peerAddress);
    }
    else
    {
      NS_ASSERT_MSG (false, "Incompatible address type: " << m_peerAddress);
    }
  }

  m_socket->SetAllowBroadcast (true);
  ScheduleTransmit (Seconds (0.));
}

void
VideoStreamServer::StopApplication ()
{
  NS_LOG_FUNCTION (this);

  if (m_socket != 0)
  {
    m_socket->Close();
    m_socket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket>> ());
    m_socket = 0;
  }

  Simulator::Cancel (m_sendEvent);
}

void 
VideoStreamServer::SetDataSize (uint32_t dataSize)
{
  NS_LOG_FUNCTION (this << dataSize);
  m_size = dataSize;
}

uint32_t
VideoStreamServer::GetDataSize (void) const
{
  NS_LOG_FUNCTION (this);
  return m_size;
}

void 
VideoStreamServer::ScheduleTransmit (Time dt)
{
  NS_LOG_FUNCTION (this << dt);
  m_sendEvent = Simulator::Schedule (dt, &VideoStreamServer::Send, this);
}

void 
VideoStreamServer::Send (void)
{
  NS_LOG_FUNCTION (this);

  NS_ASSERT (m_sendEvent.IsExpired ());

  Ptr<Packet> p = Create<Packet> (m_size);
  m_socket->Send (p);

  ScheduleTransmit (m_interval);
}

}