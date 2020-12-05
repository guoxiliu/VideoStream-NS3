/*****************************************************
*
* File:  videoStreamTest.cc
*
* Explanation:  This script modifies the tutorial first.cc
*               to test the video stream application.
*
*****************************************************/
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

//#define NS3_LOG_ENABLE

/**
 * @brief The test cases include:
 * 1. P2P network with 1 server and 1 client
 * 2. P2P network with 1 server and 2 clients
 */
#define CASE 3

NS_LOG_COMPONENT_DEFINE ("VideoStreamTest");

int
main (int argc, char *argv[])
{
  Time::SetResolution (Time::NS);
 
  bool verbose = true;
  uint32_t nWifi = 3; 

  CommandLine cmd;
  cmd.AddValue ("nWifi", "Number of wifi STA devices", nWifi);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
  cmd.Parse (argc, argv);
  
  // Time::SetResolution (Time::NS);
  LogComponentEnable ("VideoStreamClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("VideoStreamServerApplication", LOG_LEVEL_INFO);

  if (CASE == 1)
  {
    NodeContainer nodes;
    nodes.Create (2);

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));
    pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

    NetDeviceContainer devices;
    devices = pointToPoint.Install (nodes);

    InternetStackHelper stack;
    stack.Install (nodes);

    Ipv4AddressHelper address;
    address.SetBase ("10.1.1.0", "255.255.255.0");

    Ipv4InterfaceContainer interfaces = address.Assign (devices);

    VideoStreamClientHelper videoClient (interfaces.GetAddress (0), 5000);
    ApplicationContainer clientApp = videoClient.Install (nodes.Get (1));
    clientApp.Start (Seconds (0.5));
    clientApp.Stop (Seconds (100.0));

    VideoStreamServerHelper videoServer (5000);
    videoServer.SetAttribute ("MaxPacketSize", UintegerValue (1400));
    videoServer.SetAttribute ("FrameFile", StringValue ("./scratch/small.txt"));
    // videoServer.SetAttribute ("FrameSize", UintegerValue (4096));

    ApplicationContainer serverApp = videoServer.Install (nodes.Get (0));
    serverApp.Start (Seconds (0.0));
    serverApp.Stop (Seconds (100.0));

    pointToPoint.EnablePcap ("videoStream", devices.Get (1), false);
    AnimationInterface anim("p2p-1-1.xml");
    Simulator::Run ();
    Simulator::Destroy ();
  }

  else if (CASE == 2)
  {
    NodeContainer nodes;
    nodes.Create (3);
    NodeContainer n0n1= NodeContainer (nodes.Get(0), nodes.Get(1));
    NodeContainer n0n2= NodeContainer (nodes.Get(0), nodes.Get(2));

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));
    pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

    NetDeviceContainer d0d1= pointToPoint.Install (n0n1);
    NetDeviceContainer d0d2= pointToPoint.Install (n0n2);

    InternetStackHelper stack;
    stack.Install (nodes);

    Ipv4AddressHelper address;
    address.SetBase ("10.1.1.0", "255.255.255.0");
    address.Assign (d0d1);

    address.SetBase ("10.1.2.0", "255.255.255.0");
    address.Assign (d0d2);

    //Ipv4InterfaceContainer interfaces = address.Assign (devices);
    Ipv4InterfaceContainer i0i1 = address.Assign (d0d1);
    Ipv4InterfaceContainer i0i2 = address.Assign (d0d2);

    VideoStreamClientHelper videoClient1 (i0i1.GetAddress (0), 5000);
    ApplicationContainer clientApp1 = videoClient1.Install (nodes.Get (1));
    clientApp1.Start (Seconds (1.0));
    clientApp1.Stop (Seconds (100.0));

    VideoStreamClientHelper videoClient2 (i0i2.GetAddress (0), 5000);
    ApplicationContainer clientApp2 = videoClient2.Install (nodes.Get (2));
    clientApp2.Start (Seconds (0.5));
    clientApp2.Stop (Seconds (100.0));

    VideoStreamServerHelper videoServer (5000);
    videoServer.SetAttribute ("MaxPacketSize", UintegerValue (1400));
    videoServer.SetAttribute ("FrameFile", StringValue ("./scratch/small.txt"));
    // videoServer.SetAttribute ("FrameSize", UintegerValue (4096));

    ApplicationContainer serverApp = videoServer.Install (nodes.Get (0));
    serverApp.Start (Seconds (0.0));
    serverApp.Stop (Seconds (100.0));

    pointToPoint.EnablePcap ("videoStream", d0d1.Get (1), false);
    pointToPoint.EnablePcap ("videoStream", d0d2.Get (1), false);
    AnimationInterface anim("p2p-1-2.xml");
    Simulator::Run ();
    Simulator::Destroy ();
  }

  else if (CASE == 3)
  {
  NodeContainer wifiStaNodes;
  wifiStaNodes.Create (nWifi);  
  NodeContainer wifiApNode;
  wifiApNode.Create(1);   
   
  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();   
  YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();  
  phy.SetChannel (channel.Create ());  
 
  WifiHelper wifi;
  wifi.SetRemoteStationManager ("ns3::AarfWifiManager");  
 
 
  WifiMacHelper mac; 
  Ssid ssid = Ssid ("ns-3-aqiao");  
  mac.SetType ("ns3::StaWifiMac",    
               "Ssid", SsidValue (ssid),   
               "ActiveProbing", BooleanValue (false));  
 
  NetDeviceContainer staDevices;
  staDevices = wifi.Install (phy, mac, wifiStaNodes);  
 
  mac.SetType ("ns3::ApWifiMac",   
               "Ssid", SsidValue (ssid));   
 
  NetDeviceContainer apDevices;
  apDevices = wifi.Install (phy, mac, wifiApNode);   
  MobilityHelper mobility; 
 
  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));
 
    
  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",   
                             "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));   
  mobility.Install (wifiStaNodes);
   
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");  
  mobility.Install (wifiApNode);
 
  InternetStackHelper stack;
  stack.Install (wifiApNode);
  stack.Install (wifiStaNodes);   
 
  Ipv4AddressHelper address;
 
  address.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer wifiInterfaces;
  wifiInterfaces=address.Assign (staDevices);
  address.Assign (apDevices); 


                
  //UdpEchoServerHelper echoServer (9);
  VideoStreamServerHelper videoServer (5000);
  videoServer.SetAttribute ("MaxPacketSize", UintegerValue (1400));
  videoServer.SetAttribute ("FrameFile", StringValue ("./scratch/small.txt")); 
  ApplicationContainer serverApps = videoServer.Install (wifiStaNodes.Get (0));
  serverApps.Start (Seconds (0.0));
  serverApps.Stop (Seconds (100.0));
 
  //UdpEchoClientHelper echoClient (wifiInterfaces.GetAddress (0), 9);
  VideoStreamClientHelper videoClient (wifiInterfaces.GetAddress (0), 5000);
  //echoClient.SetAttribute ("MaxPackets", UintegerValue (5));
  //echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  //echoClient.SetAttribute ("PacketSize", UintegerValue (1024));
 
  ApplicationContainer clientApps =
    videoClient.Install (wifiStaNodes.Get (nWifi - 1));
  clientApps.Start (Seconds (0.5));
  clientApps.Stop (Seconds (100.0));
 
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
 
  Simulator::Stop (Seconds (10.0));
 
  phy.EnablePcap ("wifi-videoStream", apDevices.Get (0));
 

  AnimationInterface anim("wifi-1-3.xml");
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;


  }




  return 0;
}
