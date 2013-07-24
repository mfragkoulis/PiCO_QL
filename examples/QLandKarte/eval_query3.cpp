/* C++ lines of code: 23
 * x: excluded
 */
void cpp_query3(CTrackDB *c, CWptDB *w) {
  QMap<QString, CTrack*>* m = (QMap<QString, CTrack *> *)&c->getTracks();                /*  x */
  QMap<QString, CTrack*>::iterator iter;
  QList<CTrack::pt_t>::iterator it;
  std::vector<std::multimap<std::string,CTrack::pt_t*> > resultsetTracks;
  std::vector<std::multimap<std::string, CTrack::pt_t*> >::iterator iterTr;
  std::multimap<std::string, CTrack::pt_t*>::iterator iterTrP;
  std::vector<CWpt*>::iterator iterP;
  QMap<QString, CWpt*>* p = (QMap<QString, CWpt *> *)&w->getWpts();
  QMap<QString, CWpt*>::iterator iterWpts;
  std::vector<CWpt*> resultsetWpts;
  for (int i = 0; i < 10;i++) {                                                          /*  x */
    for (iter = m->begin(); iter != m->end();iter++) {
      resultsetTracks.push_back(std::multimap<std::string, CTrack::pt_t*>());
      for (it = iter.value()->getTrackPoints().begin(); it != iter.value()->getTrackPoints().end(); it++) {
	if (it->ele > 20) {
	  resultsetTracks.back().insert(std::pair<std::string, CTrack::pt_t*>(iter.value()->getName().toStdString(), &*it));
	}                                                                                /*  x */
      }                                                                                  /*  x */
    }                                                                                    /*  x */
    for (iterWpts = p->begin(); iterWpts != p->end();iterWpts++) {
      if (iterWpts.value()->ele > 20) {
	resultsetWpts.push_back(iterWpts.value());
      }                                                                                  /*  x */
    }                                                                                    /*  x */
    std::cout << "name | lon | lat | ele" << std::endl;
    for (iterTr = resultsetTracks.begin(); iterTr != resultsetTracks.end(); iterTr++) {
      for (iterTrP = (*iterTr).begin(); iterTrP != (*iterTr).end(); iterTrP++) {
	std::cout << (*iterTrP).first << " | "
		  << (*iterTrP).second->lon << " | "                                     /*  x */
		  << (*iterTrP).second->lat << " | "                                     /*  x */
		  << (*iterTrP).second->ele << " | "                                     /*  x */
		  << std::endl;                                                          /*  x */
      }                                                                                  /*  x */
    }                                                                                    /*  x */
    for (iterP = resultsetWpts.begin(); iterP != resultsetWpts.end(); iterP++) {      
      std::cout << (*iterP)->getName().toStdString() << " | "                            /*  x */
		<< (*iterP)->lon << " | "                                                /*  x */
		<< (*iterP)->lat << " | "                                                /*  x */
		<< (*iterP)->ele << " | "                                                /*  x */
		<< std::endl;                                                            /*  x */
    }                                                                                    /*  x */
    resultsetTracks.clear();                                                             /*  x */
    resultsetWpts.clear();                                                               /*  x */
  }                                                                                      /*  x */
}                                                                                        /*  x */
