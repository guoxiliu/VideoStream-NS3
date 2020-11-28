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

using namespace ns3;

//#define NS3_LOG_ENABLE

NS_LOG_COMPONENT_DEFINE ("VideoStreamTest");

int
main (int argc, char *argv[])
{
  CommandLine cmd;
  cmd.Parse (argc, argv);
  
  Time::SetResolution (Time::NS);
  LogComponentEnable ("VideoStreamClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("VideoStreamServerApplication", LOG_LEVEL_INFO);

  NodeContainer nodes;
  nodes.Create (2);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("1000Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer devices;
  devices = pointToPoint.Install (nodes);

  InternetStackHelper stack;
  stack.Install (nodes);

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");

  Ipv4InterfaceContainer interfaces = address.Assign (devices);

  VideoStreamClientHelper videoClient (5000);
  ApplicationContainer clientApp = videoClient.Install (nodes.Get (1));
  clientApp.Start (Seconds (0.0));
  clientApp.Stop (Seconds (100.0));

  VideoStreamServerHelper videoServer (interfaces.GetAddress (1), 5000);
  videoServer.SetAttribute ("MaxPacketSize", UintegerValue (1400));
  videoServer.SetAttribute ("FrameFile", StringValue ("./scratch/frameList.txt"));
  // videoServer.SetAttribute ("FrameSize", UintegerValue (4096));

  ApplicationContainer serverApp = videoServer.Install (nodes.Get (0));
  serverApp.Start (Seconds (1.0));
  serverApp.Stop (Seconds (100.0));

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
