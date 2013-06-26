void cTrackDB_cpp_query1(CTrackDB *c) 
{
  QMap<QString, CTrack*>* m = (QMap<QString, CTrack *> *)&c->getTracks();
  QMap<QString, CTrack*>::iterator iter;
  QList<CTrack::pt_t>::iterator it;
  std::vector<std::multimap<double, float> > resultset;
  std::multimap<float, std::pair<double,QMap<QString, CTrack*>::iterator> > aggregate;
  clock_t start, finish;
  double t = 0;
  std::vector<std::multimap<double, float> >::iterator rsIt;
  std::multimap<double, float>::iterator groupBy, groupByNext;
  float speed_max, currentSp, nextSp;
  double currentAz, nextAz;
  std::pair<double, QMap<QString, CTrack*>::iterator > p;
  std::multimap<float, std::pair<double, QMap<QString, CTrack*>::iterator> >::iterator aggrIt;
  double azimuth, totalDistance, ascend, descend;
  int totalTimemoving, totalTime;
  std::string name;
  CTrack *curTrack;
  for (int i = 0; i < 10; i++) {
    start = clock();
    for (iter = m->begin(); iter != m->end(); iter++) {
      resultset.push_back(std::multimap<double, float>());
      for (it = iter.value()->getTrackPoints().begin(); it != iter.value()->getTrackPoints().end(); it++) {
	resultset.back().insert(std::pair<double, float>(it->azimuth, it->speed));
      }
    }
    speed_max = -2000000;
    currentAz = 0.0;
    nextAz = 0.0;
    currentSp = -5.0;
    nextSp = -5.0;
    iter = m->begin();
    for (rsIt = resultset.begin(); rsIt != resultset.end(); rsIt++) {
      groupByNext = (*rsIt).begin();
      groupBy = (*rsIt).begin();
      groupByNext++;
      for (; groupByNext != (*rsIt).end(); groupByNext++) {
	currentAz = (*groupBy).first;
	nextAz = (*groupByNext).first;
	currentSp = (*groupBy).second;
	nextSp = (*groupByNext).second;
	if (currentAz == nextAz) {
	  if (currentSp > nextSp) {
	    if (currentSp > speed_max)
	      speed_max = currentSp;
	  } else {
	    if (nextSp > speed_max)
	      speed_max = nextSp;
	  }
	  //	std::cout << "Equal azimuths: " << currentAz << " <> " << nextAz << std::endl;
	  //	std::cout << "Speeds of equal: " << currentSp << " <> " << nextSp << std::endl;
	} else {
	  if (speed_max == -2000000)
	    speed_max = currentSp;
	  p = std::make_pair (currentAz, (QMap<QString, CTrack *>::iterator)iter);
	  aggregate.insert(std::pair<float, std::pair<double, QMap<QString, CTrack*>::iterator> >(speed_max, p));
	  //	std::cout << "Max speed: " << speed_max << ", azimuth " << currentAz << std::endl;
	  speed_max = -2000000;
	}
	groupBy++;
      }
      iter++;
    }
    if (resultset.size() > 0) {
      iter--;
      p = std::make_pair (nextAz, (QMap<QString, CTrack *>::iterator)iter);
      if (currentAz != nextAz)
	speed_max = (*groupBy).second;
      aggregate.insert(std::pair<float, std::pair<double, QMap<QString, CTrack*>::iterator > >(speed_max, p));
      //    std::cout << "Max speed: " << speed_max << ", azimuth " << nextAz << std::endl;
    }
    //std::cout << "Aggregate size: " << aggregate.size() << std::endl;
    std::cout << "name | descend | ascend | distance | "
	      << "totaltime | totaltimemoving | azimuth | "
	      << " max(speed)" << std::endl;
    for (aggrIt = aggregate.begin(); aggrIt != aggregate.end(); aggrIt++) {
      speed_max = (*aggrIt).first;
      azimuth = (*aggrIt).second.first;
      curTrack = (*aggrIt).second.second.value();
      totalTimemoving = curTrack->getTotalTimeMoving();
      totalTime = curTrack->getTotalTime();
      totalDistance = curTrack->getTotalDistance();
      ascend = curTrack->getAscend();
      descend = curTrack->getDescend();
      name = curTrack->getName().toStdString();
      std::cout << name << " | "
		<< descend << " | "
		<< ascend << " | "
		<< totalDistance << " | "
		<< totalTime << " | "
		<< totalTimemoving << " | "
		<< azimuth << " | "
		<< speed_max << " | " << std::endl;
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
