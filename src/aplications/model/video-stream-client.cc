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
}

VideoStreamClient::~VideoStreamClient ()
{
  NS_LOG_FUNCTION (this);
  m_socket = 0;
  m_socket6 = 0;
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
  if (m_socket6 == 0)
  {
      TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
      m_socket6 = Socket::CreateSocket (GetNode (), tid);
      Inet6SocketAddress local6 = Inet6SocketAddress (Ipv6Address::GetAny (), m_port);
      if (m_socket6->Bind (local6) == -1)
      {
        NS_FATAL_ERROR ("Failed to bind socket");
      }
      if (addressUtils::IsMulticast (local6))
      {
        Ptr<UdpSocket> udpSocket = DynamicCast<UdpSocket> (m_socket6);
        if (udpSocket)
        {
          udpSocket->MulticastJoinGroup (0, local6);
        }
        else
        {
          NS_FATAL_ERROR ("Error: Failed to join multicast group");
        }
      }
  }

  m_socket->SetRecvCallback (MakeCallback (&VideoStreamClient::HandleRead, this));
  m_socket6->SetRecvCallback (MakeCallback (&VideoStreamClient::HandleRead, this));
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
  if (m_socket6 != 0)
  {
    m_socket6->Close();
    m_socket6->SetRecvCallback (MakeNullCallback<void, Ptr<Socket>> ());
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
    }
    else if (Inet6SocketAddress::IsMatchingType (from))
    {
      NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s server received " << packet->GetSize () << " bytes from " << Inet6SocketAddress::ConvertFrom (from).GetIpv6 () << " port " << Inet6SocketAddress::ConvertFrom (from).GetPort ());
    }
  }
  
}

}