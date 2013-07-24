/* C++ lines of code: 49
 * x: excluded
 */
void cTrackDB_cpp_query2(CTrackDB *c) {
  QMap<QString, CTrack*>* m = (QMap<QString, CTrack *>*)&c->getTracks();            /*  x */
  QMap<QString, CTrack*>::iterator iter;
  QList<CTrack::pt_t>::iterator it;
  // heartRateBpm, elevation                                                        /*  x */
  std::vector<std::multimap<int, std::pair<float, float> > > resultset;
  // maxAvgspeed, elevation, heartRateBpm                                           /*  x */
  std::multimap<float, std::pair<float,std::pair<int, QMap<QString, CTrack*>::iterator > > > aggregate;
  std::vector<std::multimap<int, std::pair<float, float> > >::iterator rsIt;
  std::multimap<int, std::pair<float, float> >::iterator groupBy, groupByNext, groupByEnd, mIter;
  std::pair<std::multimap<int, std::pair<float, float> >::iterator, std::multimap<int, std::pair<float, float> >::iterator> ret;
  float maxAvgspeed = -100.0, currentAvgSp, currentEle = 0.0;
  int currentHr = 0;
  std::multimap<float, std::pair<float, QMap<QString, CTrack*>::iterator> > mapEle;
  std::pair<std::multimap<float, std::pair<float, QMap<QString, CTrack*>::iterator> >::iterator, std::multimap<float, std::pair<float, QMap<QString, CTrack*>::iterator> >::iterator> ret2;
  std::pair<float, std::pair<float, QMap<QString, CTrack*>::iterator> > pairEle;    /*  x */
  std::pair<float, QMap<QString, CTrack*>::iterator> pairAvg;                       /*  x */
  std::pair<int, QMap<QString, CTrack*>::iterator> pairHr;                          /*  x */
  std::multimap<float, std::pair<float, QMap<QString, CTrack*>::iterator> >::iterator mapEleIt, groupBy2;
  double totalDistance, ascend, descend;                                            /*  x */
  int totalTimemoving, totalTime, heartRate;                                        /*  x */
  float ele;                                                                        /*  x */
  std::string name;                                                                 /*  x */
  for (int i = 0; i < 10; i++) {                                                    /*  x */
    for (iter = m->begin(); iter != m->end(); iter++) {
      resultset.push_back(std::multimap<int, std::pair<float, float> >());
      for (it = iter.value()->getTrackPoints().begin(); it != iter.value()->getTrackPoints().end(); it++) {
        resultset.back().insert(std::pair<int, std::pair<float, float> >(it->heartReateBpm, std::make_pair(it->ele, it->avgspeed)));
      }                                                                             /*  x */
    }                                                                               /*  x */
    iter = m->begin();
    for (rsIt = resultset.begin(); rsIt != resultset.end();rsIt++) {
      for (mIter = (*rsIt).begin(); mIter != (*rsIt).end();) {
        currentHr = mIter->first;                                                   /*  x */
        currentAvgSp = mIter->second.second;                                        /*  x */
        currentEle = mIter->second.first;                                           /*  x */
        ret = (*rsIt).equal_range(currentHr);
        if (ret.first == ret.second) {
          if (currentAvgSp > 0) {
            pairHr = std::make_pair (currentHr, iter);                              /*  x */
            pairEle = std::make_pair (currentEle, pairHr);                          /*  x */
            aggregate.insert(std::pair<float, std::pair<float, std::pair<int, QMap<QString, CTrack*>::iterator > > >(cur
rentAvgSp, pairEle));
            maxAvgspeed = -100.0;
          }                                                                         /*  x */
        } else {
          for (groupBy = ret.first; groupBy != ret.second; groupBy++) {
            pairAvg = std::make_pair (groupBy->second.second, (QMap<QString, CTrack *>::iterator)iter);  /*  x */
            mapEle.insert(std::pair<float, std::pair<float, QMap<QString, CTrack*>::iterator > >(groupBy->second.first, 
pairAvg));
          }                                                                         /*  x */
          for (mapEleIt = mapEle.begin(); mapEleIt != mapEle.end();) {
            currentEle = mapEleIt->first;                                           /*  x */
            ret2 = mapEle.equal_range(currentEle);
            if (ret2.first == ret2.second) {
              maxAvgspeed = mapEleIt->second.first;
            } else {
              for (groupBy2 = ret2.first; groupBy2 != ret2.second; groupBy2++) {
                if (maxAvgspeed < groupBy2->second.first) {
                  maxAvgspeed = groupBy2->second.first;
                  currentEle = groupBy2->first;
                }                                                                   /*  x */
              }                                                                     /*  x */
            }                                                                       /*  x */
            if (maxAvgspeed > 0) {
              pairHr = std::make_pair (currentHr, iter);                            /*  x */
              pairEle = std::make_pair (currentEle, pairHr);                        /*  x */
              aggregate.insert(std::pair<float, std::pair<float, std::pair<int, QMap<QString, CTrack*>::iterator > > >(maxAvgspeed, pairEle));
            }                                                                       /*  x */
            mapEleIt = ret2.second;
            maxAvgspeed = -100.0;
          }                                                                         /*  x */
          mapEle.clear();
        }                                                                           /*  x */
        mIter = ret.second;
      }                                                                             /*  x */
    }                                                                               /*  x */
    std::cout << "Aggregate size: " << aggregate.size() << std::endl;               /*  x */
    std::cout << "name | descend | ascend | distance | "
              << "totaltime | totaltimemoving | "                                   /*  x */
              << "heartRateBpm | elevation | "                                      /*  x */
              << " max(avgspeed)" << std::endl;                                     /*  x */
    std::multimap<float, std::pair<float, std::pair<int, QMap<QString, CTrack*>::iterator > > >::reverse_iterator aggrRIt;
    CTrack *currentTrack;                                                           /*  x */
    for (aggrRIt = aggregate.rbegin(); aggrRIt != aggregate.rend(); aggrRIt++) {
      maxAvgspeed = (*aggrRIt).first;                                               /*  x */
      ele = (*aggrRIt).second.first;                                                /*  x */
      heartRate = (*aggrRIt).second.second.first;                                   /*  x */
      currentTrack = (*aggrRIt).second.second.second.value();                       /*  x */
      totalTimemoving = currentTrack->getTotalTimeMoving();                         /*  x */
      totalTime = currentTrack->getTotalTime();                                     /*  x */
      totalDistance = currentTrack->getTotalDistance();                             /*  x */
      ascend = currentTrack->getAscend();                                           /*  x */
      descend = currentTrack->getDescend();                                         /*  x */
      name = currentTrack->getName().toStdString();                                 /*  x */
      std::cout << name << " | "
                << descend << " | "                                                 /*  x */
                << ascend << " | "                                                  /*  x */
                << totalDistance << " | "                                           /*  x */
                << totalTime << " | "                                               /*  x */
                << totalTimemoving << " | "                                         /*  x */
                << heartRate << " | "                                               /*  x */
                << ele << " | "                                                     /*  x */
                << maxAvgspeed << " | " << std::endl;                               /*  x */
                }                                                                   /*  x */
    resultset.clear();                                                              /*  x */
    aggregate.clear();                                                              /*  x */
  }                                                                                 /*  x */
}                                                                                   /*  x */
