/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2011-2015  Regents of the University of California.
 *
 * This file is part of ndnSIM. See AUTHORS for complete list of ndnSIM authors and
 * contributors.
 *
 * ndnSIM is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * ndnSIM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ndnSIM, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 **/

// ndn-congestion-topo-plugin.cpp

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"
#include "../extensions/mircc-strategy.hpp"

namespace ns3 {

/**
 * This scenario simulates a grid topology (using topology reader module)
 *
 *   /------\                                                    /------\
 *   | Src1 |<--+                                            +-->| Dst1 |
 *   \------/    \                                          /    \------/
 *                \                                        /
 *                 +-->/------\   "bottleneck"  /------\<-+
 *                     | Rtr1 |<===============>| Rtr2 |
 *                 +-->\------/                 \------/<-+
 *
 * To run scenario and see what is happening, use the following command:
 *
 *     NS_LOG=ndn.Consumer:ndn.Producer ./waf --run=ndn-congestion-topo-plugin
 */

int
main(int argc, char* argv[])
{
  CommandLine cmd;
  cmd.Parse(argc, argv);

  AnnotatedTopologyReader topologyReader("", 25);
  topologyReader.SetFileName("topologies/topo-4-node.txt");
  topologyReader.Read();

  // Install NDN stack on all nodes
  ndn::StackHelper ndnHelper;
  ndnHelper.setPolicy("nfd::cs::lru");
  ndnHelper.setCsSize(1);
  ndnHelper.InstallAll();

  // Choosing forwarding strategy
  ndn::StrategyChoiceHelper::InstallAll<nfd::fw::MIRCCStrategy>("/");
  //ndn::StrategyChoiceHelper::InstallAll<nfd::fw::SelfLearningStrategy>("/prefix");
  //ndn::StrategyChoiceHelper::InstallAll("/prefix", "/localhost/nfd/strategy/best-route-strategy");

  // Installing global routing interface on all nodes
  ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
  ndnGlobalRoutingHelper.InstallAll();

  // Getting containers for the consumer/producer
  Ptr<Node> consumer1 = Names::Find<Node>("Src1");

  Ptr<Node> producer1 = Names::Find<Node>("Dst1");

  ndn::AppHelper consumerHelper("ns3::ndn::ConsumerMIRCC");
  //ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
  consumerHelper.SetAttribute("MaxSeq", StringValue("10000000")); // 100 interests a second
  consumerHelper.SetAttribute("start_ms", StringValue("1")); // 100 interests a second
  consumerHelper.SetPrefix("/dst1");
  consumerHelper.Install(consumer1);


    ndn::AppHelper consumerHelper2("ns3::ndn::ConsumerMIRCC");
    consumerHelper2.SetAttribute("MaxSeq", StringValue("10000000")); // 100 interests a second
  consumerHelper2.SetAttribute("start_ms", StringValue("3000")); // 100 interests a second
  consumerHelper2.SetPrefix("/dst1");
  consumerHelper2.Install(consumer1);
consumerHelper2.Install(consumer1);
consumerHelper2.Install(consumer1);
/*
    ndn::AppHelper consumerHelper3("ns3::ndn::ConsumerMIRCC");
  consumerHelper3.SetAttribute("MaxSeq", StringValue("10000000")); // 100 interests a second
  consumerHelper3.SetAttribute("start_ms", StringValue("1500")); // 100 interests a second
  consumerHelper3.SetPrefix("/dst3");
  consumerHelper3.Install(consumer1);

    ndn::AppHelper consumerHelper4("ns3::ndn::ConsumerMIRCC");
  consumerHelper4.SetAttribute("MaxSeq", StringValue("10000000")); // 100 interests a second
  consumerHelper4.SetAttribute("start_ms", StringValue("1500")); // 100 interests a second
  consumerHelper4.SetPrefix("/dst4");
  consumerHelper4.Install(consumer1);*/


  

  ndn::AppHelper producerHelper("ns3::ndn::Producer");
  producerHelper.SetAttribute("PayloadSize", StringValue("1024"));
  ndnGlobalRoutingHelper.AddOrigins("/dst1", producer1);
  producerHelper.SetPrefix("/dst1");
  producerHelper.Install(producer1);
/*
  ndn::AppHelper producerHelper2("ns3::ndn::Producer");
  producerHelper2.SetAttribute("PayloadSize", StringValue("1024"));
  ndnGlobalRoutingHelper.AddOrigins("/dst2", producer1);
  producerHelper2.SetPrefix("/dst2");
  producerHelper2.Install(producer1);

  ndn::AppHelper producerHelper3("ns3::ndn::Producer");
  producerHelper3.SetAttribute("PayloadSize", StringValue("1024"));
  ndnGlobalRoutingHelper.AddOrigins("/dst3", producer1);
  producerHelper3.SetPrefix("/dst3");
  producerHelper3.Install(producer1);

  ndn::AppHelper producerHelper4("ns3::ndn::Producer");
  producerHelper4.SetAttribute("PayloadSize", StringValue("1024"));
  ndnGlobalRoutingHelper.AddOrigins("/dst4", producer1);
  producerHelper4.SetPrefix("/dst4");
  producerHelper4.Install(producer1);
*/
   // ndnGlobalRoutingHelper.AddOrigins("/dst2", producer1);
  //producerHelper.SetPrefix("/dst2");
  //producerHelper.Install(producer1);

  //ndnGlobalRoutingHelper.AddOrigins("/dst3", producer1);
  //producerHelper.SetPrefix("/dst3");
  //producerHelper.Install(producer1);

  //ndnGlobalRoutingHelper.AddOrigins("/dst4", producer1);
  //producerHelper.SetPrefix("/dst4");
  //producerHelper.Install(producer1);
  // Calculate and install FIBs
  ndn::GlobalRoutingHelper::CalculateRoutes();

  Simulator::Stop(Seconds(10));

  ndn::L3RateTracer::InstallAll("out1.txt",Seconds(0.1));
  ndn::AppDelayTracer::InstallAll("out2.txt");
  L2RateTracer::InstallAll("out3",Seconds(1));


  Simulator::Run();
  Simulator::Destroy();

  return 0;
}

} // namespace ns3

int
main(int argc, char* argv[])
{
  return ns3::main(argc, argv);
}
