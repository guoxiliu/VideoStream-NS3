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

  m_bufferEvent = Simulator::Schedule (Seconds (m_initialDelay)+MicroSeconds (100), &VideoStreamClient::ReadFromBuffer, this);
}

void
VideoStreamClient::StopApplication ()
{
  NS_LOG_FUNCTION (this);

  if (m_socket != 0)
  {
    m_socket->Close ();
    m_socket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket>> ());
  }
}

uint32_t 
VideoStreamClient::ReadFromBuffer (void)
{
  if (m_currentBufferSize < m_frameRate) 
  {
    m_bufferEvent = Simulator::Schedule (Seconds (1.0), &VideoStreamClient::ReadFromBuffer, this);
    return (-1);
  }
  else
  {
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
      NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s server received " << packet->GetSize () << " bytes from " << InetSocketAddress::ConvertFrom (from).GetIpv4 () << " port " << InetSocketAddress::ConvertFrom (from).GetPort ());

      m_currentBufferSize++;
    }
  }
}

}