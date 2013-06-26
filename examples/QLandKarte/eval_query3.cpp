void cpp_query3(CTrackDB *c, CWptDB *w) {
  clock_t start, finish;
  double t = 0;
  QMap<QString, CTrack*>* m = (QMap<QString, CTrack *> *)&c->getTracks();
  QMap<QString, CTrack*>::iterator iter;
  QList<CTrack::pt_t>::iterator it;
  std::vector<std::multimap<std::string,CTrack::pt_t*> > resultsetTracks;
  std::vector<std::multimap<std::string, CTrack::pt_t*> >::iterator iterTr;
  std::multimap<std::string, CTrack::pt_t*>::iterator iterTrP;
  std::vector<CWpt*>::iterator iterP;
  QMap<QString, CWpt*>* p = (QMap<QString, CWpt *> *)&w->getWpts();
  QMap<QString, CWpt*>::iterator iterWpts;
  std::vector<CWpt*> resultsetWpts;
  for (int i = 0; i < 10;i++) {
    start = clock();
    for (iter = m->begin(); iter != m->end();iter++) {
      resultsetTracks.push_back(std::multimap<std::string, CTrack::pt_t*>());
      for (it = iter.value()->getTrackPoints().begin();
	   it != iter.value()->getTrackPoints().end(); it++) {
	if (it->ele > 20) {
	  resultsetTracks.back().insert(std::pair<std::string, CTrack::pt_t*>(iter.value()->getName().toStdString(), &*it));
	  //	std::cout << "Inserted trackpoint with elevation: "
	  //  << it->ele << std::endl;
	}
      }
      //    std::cout << "Total points evaluated: "
      //        << (*iter).second->getStdTrackPoints()->size() << std::endl;
    }
    for (iterWpts = p->begin(); iterWpts != p->end();iterWpts++) {
      if (iterWpts.value()->ele > 20) {
	resultsetWpts.push_back(iterWpts.value());
	//   std::cout << "Inserted point with elevation: "
	//	<< (*iterWpts).second->ele << std::endl;
      }
    }
    std::cout << "name | lon | lat | ele" << std::endl;
    for (iterTr = resultsetTracks.begin(); iterTr != resultsetTracks.end(); iterTr++) {
      for (iterTrP = (*iterTr).begin(); iterTrP != (*iterTr).end(); iterTrP++) {
	std::cout << (*iterTrP).first << " | "
		  << (*iterTrP).second->lon << " | "
		  << (*iterTrP).second->lat << " | "
		  << (*iterTrP).second->ele << " | "
		  << std::endl;
      }
    }
    for (iterP = resultsetWpts.begin(); iterP != resultsetWpts.end(); iterP++) {
      
      std::cout << (*iterP)->getName().toStdString() << " | "
		<< (*iterP)->lon << " | "
		<< (*iterP)->lat << " | "
		<< (*iterP)->ele << " | "
		<< std::endl;
    }
    finish = clock();
    t += ((double)finish - (double)start)/CLOCKS_PER_SEC;
    resultsetTracks.clear();
    resultsetWpts.clear();
  }
  std::cout << "Query executed in " << t/10 << " seconds."
	    << std::endl;
}
