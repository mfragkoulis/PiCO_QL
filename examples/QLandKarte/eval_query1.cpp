/* C++ lines of code: 37
 * x: excluded
 */
float fn(float max, float current) {
  return max >= current ? max : current;
}                                                    /*  x */
                                                     /*  x */
void cTrackDB_cpp_query1(CTrackDB *c) {
  QMap<QString, CTrack*>* m = (QMap<QString, CTrack *> *)&c->getTracks();
  QMap<QString, CTrack*>::iterator iter;
  QList<CTrack::pt_t>::iterator it;
  std::vector<std::multimap<double, float> > resultset;
  std::multimap<float, std::pair<double,QMap<QString, CTrack*>::iterator> > aggregate;
  std::vector<std::multimap<double, float> >::iterator rsIt;
  std::multimap<double, float>::iterator groupBy;
  std::pair<std::multimap<double,float>::iterator, std::multimap<double,float>::iterator> ret;
  float speed_max;
  double currentAz;
  std::pair<double, QMap<QString, CTrack*>::iterator > p;
  std::multimap<float, std::pair<double, QMap<QString, CTrack*>::iterator> >::iterator aggrIt;
  double azimuth, totalDistance, ascend, descend;    /*  x */
  int totalTimemoving, totalTime;                    /*  x */
  std::string name;                                  /*  x */
  CTrack *curTrack;                                  /*  x */
  for (int i = 0; i < 10; i++) {                     /*  x */
    for (iter = m->begin(); iter != m->end(); iter++) {
      resultset.push_back(std::multimap<double, float>());
      for (it = iter.value()->getTrackPoints().begin(); it != iter.value()->getTrackPoints().end(); it++) {
        resultset.back().insert(std::pair<double, float>(it->azimuth, it->speed));
      }                                              /*  x */
    }                                                /*  x */
    speed_max = -2000000;
    iter = m->begin();
    for (rsIt = resultset.begin(); rsIt != resultset.end(); rsIt++) {
      for (groupBy = (*rsIt).begin(); groupBy != (*rsIt).end();) {
        currentAz = (*groupBy).first;
        ret = (*rsIt).equal_range(currentAz);
        if (ret.first == ret.second)
          speed_max = ret.first->second;
        else {
          for (std::multimap<double, float>::iterator it = ret.first; it != ret.second; it++)
            speed_max = fn(speed_max, it->second);
        }                                            /*  x */
        p = std::make_pair (currentAz, (QMap<QString, CTrack *>::iterator)iter);
        aggregate.insert(std::pair<float, std::pair<double, QMap<QString, CTrack*>::iterator> >(speed_max, p));
        speed_max = -2000000;
        groupBy = ret.second;
      }                                              /*  x */
      iter++;
    }                                                /*  x */
    std::cout << "name | descend | ascend | distance | "
              << "totaltime | totaltimemoving | azimuth | "  /*  x */
              << " max(speed)" << std::endl;                 /*  x */
    for (aggrIt = aggregate.begin(); aggrIt != aggregate.end(); aggrIt++) {
      speed_max = (*aggrIt).first;                           /*  x */
      azimuth = (*aggrIt).second.first;                      /*  x */
      curTrack = (*aggrIt).second.second.value();            /*  x */
      totalTimemoving = curTrack->getTotalTimeMoving();      /*  x */
      totalTime = curTrack->getTotalTime();                  /*  x */
      totalDistance = curTrack->getTotalDistance();          /*  x */
      ascend = curTrack->getAscend();                        /*  x */
      descend = curTrack->getDescend();                      /*  x */
      name = curTrack->getName().toStdString();              /*  x */
      std::cout << name << " | "
                << descend << " | "                          /*  x */
                << ascend << " | "                           /*  x */
                << totalDistance << " | "                    /*  x */
                << totalTime << " | "                        /*  x */
                << totalTimemoving << " | "                  /*  x */
                << azimuth << " | "                          /*  x */
                << speed_max << " | " << std::endl;          /*  x */
    }                                                        /*  x */
}                                                            /*  x */
