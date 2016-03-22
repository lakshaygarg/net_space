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
#include "ns3/onoff-application.h"
#include "ns3/stats-module.h"
#include "ns3/flow-monitor-module.h"

#include "ns3/object.h"
#include "ns3/uinteger.h"
#include "ns3/traced-value.h"
#include "ns3/trace-source-accessor.h"

/*void
ReceivePacket(int s)
{
  s = 1;
}*/

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE ("Assignment");

static const uint32_t totalTxBytes = 2000000;
//static uint32_t currentTxBytes = 0;

static const uint32_t writeSize = 1000;
uint8_t data[writeSize];
static const int simT = 50;

int main (int argc, char *argv[])
{
  uint32_t rtsThreshold = 512;
  string manager = "ns3::MinstrelWifiManager";
  string outputFileName = "minstrel";
  int ap1_x = 0;
  int ap1_y = 0;
  int sta1_x = 250;
  int sta1_y = 0;
  int ap2_x =  500;
  int ap2_y = 0;

 /* for(uint32_t i = 0; i < writeSize; ++i)
    {
      char m = toascii (97 + i % 25);
      data[i] = m;
    }
*/
  NodeContainer nodes;
  nodes.Create (3);

  Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(1000));


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
  Ipv4InterfaceContainer i = address.Assign (devices);
  Ipv4Address sinkAddress = i.GetAddress (1);
  uint16_t port = 9 ;


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


  PacketSinkHelper sink ("ns3::TcpSocketFactory", InetSocketAddress (sinkAddress, port));
  ApplicationContainer apps_sink = sink.Install (nodes.Get (1));

  OnOffHelper onoff ("ns3::TcpSocketFactory", InetSocketAddress (sinkAddress, port));
  onoff.SetConstantRate (DataRate ("54Mb/s"), 500);
  onoff.SetAttribute ("StartTime", TimeValue (Seconds (0.5)));
  onoff.SetAttribute ("StopTime", TimeValue (Seconds (simT)));
  ApplicationContainer apps_source1 = onoff.Install (nodes.Get (0));
  ApplicationContainer apps_source2 = onoff.Install (nodes.Get (2));

  apps_sink.Start (Seconds (0.5));
  apps_source1.Start (Seconds (0.5));
  apps_source2.Start (Seconds (0.5));

  apps_sink.Stop (Seconds (simT));
  apps_source1.Stop (Seconds (simT));
  apps_source2.Stop (Seconds (simT));

  AsciiTraceHelper ascii , asciiI;

  Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream ("TestTraceWifi.tr");
  Ptr<OutputStreamWrapper> streamI = asciiI.CreateFileStream ("TestTraceInternet.tr");
  wifiPhy.EnableAsciiAll (stream);
  wifiPhy.EnableAsciiAll (stream);
  wifiPhy.EnablePcap ("TestTraceWifi" , devices);
  //AnimationInterface anim ("assig.xml");

  //Config::ConnectWithoutContext("/NodeList/*/ApplicationList/*/$ns3::PacketSink/Rx" , MakeCallback(&ReceivePacket));
 // Config::ConnectWithoutContext("/NodeList/*/ApplicationList/*/$ns3::OnOffApplication/Tx", MakeCallback(&ReceivePacket));

  Simulator::Stop (Seconds (simT));
  Simulator::Run ();
  Simulator::Destroy ();

  cout<<"Simulation Complete";

}
  

  
  



