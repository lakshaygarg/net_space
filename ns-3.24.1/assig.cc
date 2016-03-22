/* Assignment-3 IEEE 802.11 with tcp
Topology N0------N1-------N2
*/

#include <iostream>
#include <fstream>
#include <string>

#include "ns3/core-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ipv4-global-routing-helper.h"


#include "ns3/core-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/applications-module.h"
#include "ns3/stats-module.h"
#include "ns3/flow-monitor-module.h"

#include "ns3/object.h"
#include "ns3/uinteger.h"
#include "ns3/traced-value.h"
#include "ns3/trace-source-accessor.h"

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE ("Assignment");

static const uint32_t totalTxBytes = 2000000;
static uint32_t currentTxBytes = 0;

static const uint32_t writeSize = 1000;
uint8_t data[writeSize];

int main (int argc, char *argv[])
{
  uint32_t rtsThreshold = 512;
  std::string manager = "ns3::MinstrelWifiManager";
  std::string outputFileName = "minstrel";
  int ap1_x = 0;
  int ap1_y = 0;
  int sta1_x = 250;
  int sta1_y = 0;
  int ap2_x =  500
  int ap2_y = 0;
  int simT = 50;

  for(uint32_t i = 0; i < writeSize; ++i)
    {
      char m = toascii (97 + i % 25);
      data[i] = m;
    }

  NodeContainer nodes;
  nodes.Create (3);


  WifiHelper wifi = WifiHelper::Default ();
  wifi.SetStandard (WIFI_PHY_STANDARD_80211a);
  NqosWifiMacHelper wifiMac = NqosWifiMacHelper::Default ();
  YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();
  YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();

  wifiPhy.SetChannel (wifiChannel.Create ());
  wifiMac.SetType ("ns3::AdhocWifiMac");


  wifi.SetRemoteStationManager (manager, "RtsCtsThreshold", UintegerValue (rtsThreshold));
  NetDeviceContainer devices = wifi.Install ( wifiPhy , wifiMac , nodes);

  //Configure the IP stack
  InternetStackHelper stack;
  stack.Install (nodes);
  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  address.Assign (devices);
  Ipv4Address sinkAddress = i.GetAddress (1);
  uint16_t port1 = 9 , port2 = 10 ;


  // Configure the mobility.
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  //Initial position of AP and STA
  positionAlloc->Add (Vector (ap1_x, ap1_y, 0.0));
  positionAlloc->Add (Vector (sta1_x, sta1_y, 0.0));
  positionAlloc->Add (Vector (ap2_x,ap2_y,0));

  mobility.SetPositionAllocator (positionAlloc);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (nodes);


  PacketSinkHelper sink1 ("ns3::TcpSocketFactory", InetSocketAddress (sinkAddress, port1));
  PacketSinkHelper sink2 ("ns3::TcpSocketFactory", InetSocketAddress (sinkAddress, port2));
  ApplicationContainer apps_sink2 = sink2.Install(nodes.Get(1));
  ApplicationContainer apps_sink1 = sink1.Install (nodes.Get (1));

  OnOffHelper onoff1 ("ns3::TcpSocketFactory", InetSocketAddress (sinkAddress, port1));
  onoff1.SetConstantRate (DataRate ("54Mb/s"), 1000);
  onoff1.SetAttribute ("StartTime", TimeValue (Seconds (0.5)));
  onoff1.SetAttribute ("StopTime", TimeValue (Seconds (simuT)));
  ApplicationContainer apps_source1 = onoff1.Install (nodes.Get (0));

  OnOffHelper onoff2 ("ns3::TcpSocketFactory", InetSocketAddress (sinkAddress, port2));
  onoff2.SetConstantRate (DataRate ("54Mb/s"), 1000);
  onoff2.SetAttribute ("StartTime", TimeValue (Seconds (0.5)));
  onoff2.SetAttribute ("StopTime", TimeValue (Seconds (simuT)));
  ApplicationContainer apps_source2 = onoff2.Install (nodes.Get (2));

  apps_sink1.Start (Seconds (0.5));
  apps_sink2.Start (Seconds (0.5));
  apps_source1.Start (Seconds (0.5));
  apps_source2.Start (Seconds (0.5));

  apps_sink1.Stop (Seconds (simT));
  apps_sink2.Stop (Seconds (simT));
  apps_source1.Stop (Seconds (simT));
  apps_source2.Stop (Seconds (simT));


  Simulator::Stop (Seconds (simT));
  Simulator::Run ();
  Simulator::Destroy ();


  

  
  



