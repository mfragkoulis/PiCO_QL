void cTrackDB_cpp_query2(CTrackDB *c)
{
  int countTrack = 0;
  QMap<QString, CTrack*>* m = (QMap<QString, CTrack *>*)&c->getTracks();
  QMap<QString, CTrack*>::iterator iter;
  QList<CTrack::pt_t>::iterator it;
  // heartRateBpm, elevation
  std::vector<std::multimap<int, std::pair<float, float> > > resultset;
  // heartRateBpm, elevation, avgspeed
  std::map<int, std::multimap<float, std::pair<float, QMap<QString, CTrack*>::iterator> > > interim;
  // maxAvgspeed, elevation, heartRateBpm
  std::multimap<float, std::pair<float,std::pair<int, QMap<QString, CTrack*>::iterator > > > aggregate;
  clock_t start, finish;
  double t = 0;
  std::vector<std::multimap<int, std::pair<float, float> > >::iterator rsIt;
  std::multimap<int, std::pair<float, float> >::iterator groupBy, groupByNext, groupByEnd;
  float maxAvgspeed = -100.0, currentAvgSp, nextAvgSp;
  int currentHr = 0, nextHr = 0;
  float currentEle = 0.0, nextEle = 0.0;
  std::multimap<float, std::pair<float, QMap<QString, CTrack*>::iterator> > mapEle;
  std::pair<float, std::pair<float, QMap<QString, CTrack*>::iterator> > pairEle;
  std::pair<float, QMap<QString, CTrack*>::iterator> pairAvg;
  bool inserted = false;
  std::pair<int, QMap<QString, CTrack*>::iterator> pairHr;
  std::map<int, std::multimap<float, std::pair<float, QMap<QString, CTrack*>::iterator> > >::iterator interimIt;
  std::multimap<float, std::pair<float, QMap<QString, CTrack*>::iterator> >::iterator mapEleIt, mapEleItNext;
  QMap<QString, CTrack*>::iterator trackMax = NULL;
  int mapEleSum = 0;
  int ele_equal = 0;
  double totalDistance, ascend, descend;
  int totalTimemoving, totalTime, heartRate;
  float ele;
  std::string name;
  for (int i = 0; i < 10; i++) {
    start = clock();
    for (iter = m->begin(); iter != m->end(); iter++) {
        std::cout << "Track: " << countTrack++ << std::endl;
      resultset.push_back(std::multimap<int, std::pair<float, float> >());
      for (it = iter.value()->getTrackPoints().begin(); it != iter.value()->getTrackPoints().end(); it++) {
	resultset.back().insert(std::pair<int, std::pair<float, float> >(it->heartReateBpm, std::make_pair(it->ele, it->avgspeed)));
	      std::cout << "Heartrate: " << it->heartReateBpm
			<< ", elevation: " << it->ele 
		<< std::endl;
      }
    }
      std::cout << "Resultset size: " << resultset.size() << std::endl;
    //if (resultset.size() > 0)
    // std::cout << "Track points size: " << resultset.back().size() << std::endl;
    maxAvgspeed = -100.0;
    currentHr = 0; 
    nextHr = 0;
    currentEle = 0.0;
    nextEle = 0.0;
    currentAvgSp = -50.0;
    nextAvgSp = -50.0;
    inserted = false;
    iter = m->begin();
    for (rsIt = resultset.begin(); rsIt != resultset.end(); rsIt++) {
      groupByNext = ((*rsIt).begin())++;
      groupBy = (*rsIt).begin();
      groupByEnd = ((*rsIt).end())--;
      groupByNext++;
      for (; groupByNext != (*rsIt).end(); groupByNext++) {
	currentHr = (*groupBy).first;
	nextHr = (*groupByNext).first;
	currentEle = (*groupBy).second.first;
	nextEle = (*groupByNext).second.first;
	currentAvgSp = (*groupBy).second.second;
	if (currentHr == nextHr) {
	  pairAvg = std::make_pair (currentAvgSp, (QMap<QString, CTrack *>::iterator)iter);
	  mapEle.insert(std::pair<float, std::pair<float, QMap<QString, CTrack*>::iterator> >(currentEle, pairAvg));
	  inserted = true;
	  std::cout << "Equal heartrates: " << currentHr << " == " << nextHr << std::endl;
	  std::cout << "Inserted in MAPELE elevation of equal heartrates: " << currentEle << ", next: " 
	    << nextEle << std::endl;
	} else {
	  // Hr <>
	  if (!inserted) {
	    pairAvg = std::make_pair (currentAvgSp, (QMap<QString, CTrack *>::iterator)iter);
	    mapEle.insert(std::pair<float, std::pair<float, QMap<QString, CTrack*>::iterator > >(currentEle, pairAvg));
	    std::cout << "Inserted in MAPELE heartrate: " << currentHr << ", elevation: " << currentEle << " - no equal." << std::endl;
	  } else {
	    inserted = false;
	    pairAvg = std::make_pair (currentAvgSp, (QMap<QString, CTrack *>::iterator)iter);
	    mapEle.insert(std::pair<float, std::pair<float, QMap<QString, CTrack*>::iterator> >(currentEle, pairAvg));
	    	  std::cout << "Inserted in MAPELE heartrate: " 
	    	    << currentHr << ", elevation: " 
	    	    << currentEle 
	    	    << " - last of equal." << std::endl;
	  }
	  std::cout << "Different heartrates: " << currentHr << " <> " << nextHr << std::endl;
	  std::cout << "Elevations of different heartrates: " << currentEle << " || " << nextEle << std::endl;
	  interim.insert(std::pair<int, std::multimap<float, std::pair<float, QMap<QString, CTrack*>::iterator> > >(currentHr, mapEle));
	  mapEle.clear();
	  std::cout << "Inserted in INTERIM heartrate: " << currentHr << std::endl;
	}
	groupBy++;
      }

      pairAvg = std::make_pair ((*groupBy).second.second, (QMap<QString, CTrack *>::iterator)iter);
      mapEle.insert(std::pair<float, std::pair<float, QMap<QString, CTrack*>::iterator> >(nextEle, pairAvg));
      //pairAvg = make_pair (nextEle, (std::pair<QString, CTrack *>*)&*iter);
      interim.insert(std::pair<int, std::multimap<float, std::pair<float, QMap<QString, CTrack*>::iterator> > >(nextHr, mapEle));
      //std::cout << "Last heartrate: " << nextHr << ", elevation " << nextEle << ", avgspeed " << (*groupBy).second.second << std::endl;
      iter++;
    }
    std::cout << "Interim size: " << interim.size() << std::endl;
    trackMax = NULL;
    mapEleSum = 0;
    ele_equal = 0;
    for (interimIt = interim.begin(); interimIt != interim.end(); interimIt++) {
      currentHr = (*interimIt).first;
      mapEleItNext = (*interimIt).second.begin();
      mapEleIt = (*interimIt).second.begin();
      mapEleItNext++;
      ele_equal = 0;
      //std::cout << "For heartrate: " << currentHr 
      //      << ", Mapele size: " 
      //      << (*interimIt).second.size() << std::endl;
      mapEleSum += (*interimIt).second.size();
      for (; mapEleItNext != (*interimIt).second.end(); mapEleItNext++) {
	currentEle = (*mapEleIt).first;
	nextEle = (*mapEleItNext).first;
	currentAvgSp = (*mapEleIt).second.first;
	nextAvgSp = (*mapEleItNext).second.first;
	if (currentEle == nextEle) {
	  ele_equal = 1;
	  if (currentAvgSp > nextAvgSp) {
	    if (currentAvgSp > maxAvgspeed) {
	      maxAvgspeed = currentAvgSp;
	      trackMax = (*mapEleIt).second.second;
	    }
	  } else {
	    if (nextAvgSp > maxAvgspeed) {
	      maxAvgspeed = nextAvgSp;
	      trackMax = (*mapEleItNext).second.second;
	    }
	  }
	  //std::cout << "Equal elevations: " << currentEle << " == " << nextEle << std::endl;
	  //std::cout << "Avgspeeds of equal elevations: " << currentAvgSp << " || " << nextAvgSp << std::endl;
	} else {
	  // Ele <>
	  ele_equal = -1;
	  //std::cout << "DIFFERENT ELEVATIONS: " << currentEle << " <> " << nextEle << std::endl;
	  if (maxAvgspeed == -100.0) {
	    maxAvgspeed = currentAvgSp;
	    trackMax = (*mapEleIt).second.second;
	  }
	  if (maxAvgspeed > 0) {
	    pairHr = std::make_pair (currentHr, (QMap<QString, CTrack *>::iterator)trackMax);
	    pairEle = make_pair (currentEle, pairHr);
	    aggregate.insert(std::pair<float, std::pair<int, std::pair<float, QMap<QString, CTrack*>::iterator > > >(maxAvgspeed, pairEle));
	    std::cout << "Max avgspeed " << maxAvgspeed << ", heart rate: " << currentHr << ", elevation " << currentEle << std::endl;
	    maxAvgspeed = -100.0;
	  }
	}
	std::cout << "Equal heartrates: " << (*interimIt).first << std::endl;
	std::cout << "Different elevations: " << currentEle << " <> " << nextEle << std::endl;
	std::cout << "Avgspeeds of equal heartrates: " << currentAvgSp << " || " << nextAvgSp << std::endl;
	mapEleIt++;
      }

      if (ele_equal == 1) {
	if (maxAvgspeed > 0) {
	  pairHr = std::make_pair (currentHr, (QMap<QString, CTrack *>::iterator)trackMax);
	  pairEle = std::make_pair (currentEle, pairHr);
	  aggregate.insert(std::pair<float, std::pair<int, std::pair<float, QMap<QString, CTrack*>::iterator> > >(maxAvgspeed, pairEle));
	  //std::cout << "Last of equal Max avgspeed " << maxAvgspeed << ", heart rate: " << currentHr << ", elevation " << currentEle << std::endl;
	}
      } else {
	if (ele_equal == 0) {
	  maxAvgspeed = (*(mapEleIt--)).second.first;
	  currentEle = (*mapEleIt).first;
	  trackMax = (*mapEleIt).second.second;
	  //std::cout << "Last and only elevation in MapEle"
	  //  << std::endl;
	} else if (ele_equal == -1) {
	  maxAvgspeed = (*mapEleIt).second.first;
	  currentEle = (*mapEleIt).first;
	  trackMax = (*mapEleIt).second.second;
	  //std::cout << "Last elevation is different - register."
	  //  << std::endl;
	}
	if (maxAvgspeed > 0) {
	  pairHr = std::make_pair (currentHr, (QMap<QString, CTrack *>::iterator)trackMax);
	  pairEle = std::make_pair (currentEle, pairHr);
	  aggregate.insert(std::pair<float, std::pair<int, std::pair<float, QMap<QString, CTrack*>::iterator > > >(maxAvgspeed, pairEle));
	    std::cout << "Last of no equal Max avgspeed " << maxAvgspeed << ", heart rate: " << currentHr << ", elevation " << currentEle << std::endl;
	  maxAvgspeed = -100.0;
	}
      }
    }

      std::cout << "Total of grouped elevation points: " 
    	    << mapEleSum << std::endl;
    std::cout << "Aggregate size: " << aggregate.size() << std::endl;
    std::cout << "name | descend | ascend | distance | "
	      << "totaltime | totaltimemoving | "
	      << "heartRateBpm | elevation | "
	      << " max(avgspeed)" << std::endl;
    std::multimap<float, std::pair<float, std::pair<int, QMap<QString, CTrack*>::iterator > > >::reverse_iterator aggrRIt;
    CTrack *currentTrack;
    //  std::multimap<float, std::pair<QString, CTrack*>* >::iterator iterEle;
    for (aggrRIt = aggregate.rbegin(); aggrRIt != aggregate.rend(); aggrRIt++) {
      maxAvgspeed = (*aggrRIt).first;
      ele = (*aggrRIt).second.first;
      //    for (iterEle = (*aggrRIt).second.second.begin(); iterEle != (*aggrRIt).second.second.end(); iterEle++) {
      heartRate = (*aggrRIt).second.second.first;
      currentTrack = (*aggrRIt).second.second.second.value();
      totalTimemoving = currentTrack->getTotalTimeMoving();
      totalTime = currentTrack->getTotalTime();
      totalDistance = currentTrack->getTotalDistance();
      ascend = currentTrack->getAscend();
      descend = currentTrack->getDescend();
      name = currentTrack->getName().toStdString();
      std::cout << name << " | "
		<< descend << " | "
		<< ascend << " | "
		<< totalDistance << " | "
		<< totalTime << " | "
		<< totalTimemoving << " | "
		<< heartRate << " | "
		<< ele << " | "
		<< maxAvgspeed << " | " << std::endl;
		}
    finish = clock();
    t += ((double)finish - (double)start)/CLOCKS_PER_SEC;
    resultset.clear();
    aggregate.clear();
  }
  std::cout << "Query took " << t/10 
	    << " seconds to execute."
	    << std::endl;
}
