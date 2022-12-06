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



int
main(int argc, char* argv[])
{
  CommandLine cmd;
  cmd.Parse(argc, argv);

  AnnotatedTopologyReader topologyReader("", 25);
  topologyReader.SetFileName("topologies/Slingshot.txt");
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
  Ptr<Node> producer2 = Names::Find<Node>("Dst2");


  ndn::AppHelper consumerHelper("ns3::ndn::ConsumerMIRCC");
  consumerHelper.SetAttribute("MaxSeq", StringValue("1000000000")); // 100 interests a second
  consumerHelper.SetPrefix("/d1");
  consumerHelper.SetAttribute("logtms",StringValue("100"));
  consumerHelper.SetAttribute("appid",StringValue("1"));
  consumerHelper.SetAttribute("start_ms",StringValue("0"));
  consumerHelper.SetAttribute("end_ms",StringValue("10000"));
  consumerHelper.Install(consumer1);


  consumerHelper.SetPrefix("/d2");
  consumerHelper.SetAttribute("appid",StringValue("2"));
  consumerHelper.SetAttribute("start_ms",StringValue("1500"));
  consumerHelper.SetAttribute("end_ms",StringValue("5000"));
  consumerHelper.SetAttribute("greedy_ms",StringValue("3000"));
  consumerHelper.SetAttribute("greedy_rate",StringValue("10000000"));
  consumerHelper.Install(consumer1);
  





  ndn::AppHelper producerHelper("ns3::ndn::Producer");
  producerHelper.SetAttribute("PayloadSize", StringValue("1024"));
  producerHelper.SetPrefix("/d1");
  producerHelper.Install(producer1);
  producerHelper.Install(producer2);
  producerHelper.SetPrefix("/d2");
  producerHelper.Install(producer2);

  ndnGlobalRoutingHelper.AddOrigins("/d1", producer1);
  ndnGlobalRoutingHelper.AddOrigins("/d1", producer2);
  ndnGlobalRoutingHelper.AddOrigins("/d2", producer2);

  ndn::GlobalRoutingHelper::CalculateRoutes();

  Simulator::Stop(Seconds(5));

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
