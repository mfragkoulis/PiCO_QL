void cpp_query3() {                                         
  clock_t start, finish;                                    
  double t = 0;                                             
  *stdMapTracks = cTrackDB->getTracks().toStdMap();         
  std::map<QString, CTrack*>::iterator iter;                
  std::list<CTrack::pt_t>::iterator it;                     
  std::vector<std::multimap<std::string,CTrack::pt_t*> > resultsetTracks;
  std::vector<std::multimap<std::string, CTrack::pt_t*> >::iterator iterTr;                                            
  std::multimap<std::string, CTrack::pt_t*>::iterator iterTrP;
  std::vector<CWpt*>::iterator iterP;                       
  std::map<QString, CWpt*>::iterator iterWpts;              
  std::vector<CWpt*> resultsetWpts;                         
  for (int i = 0; i < 10;i++) {                             
    start = clock();                                        
    for (iter = stdMapTracks->begin(); iter != stdMapTracks->end();iter++) {                                           
      resultsetTracks.push_back(std::multimap<std::string, CTrack::pt_t*>());                                          
      for (it = (*iter).second->getStdTrackPoints()->begin();
           it != (*iter).second->getStdTrackPoints()->end(); it++) {
        if (it->ele > 20) {                                 
          resultsetTracks.back().insert(std::pair<std::string, CTrack::pt_t*>((*iter).second->getName().toStdString(), &*it));
          //    std::cout << "Inserted trackpoint with elevation: "
          //  << it->ele << std::endl;                      
        }                                                   
      }                                                     
      //    std::cout << "Total points evaluated: "         
      //        << (*iter).second->getStdTrackPoints()->size() << std::endl;                                           
    }                                                       
    *stdWaypts = cWptDB->getWpts().toStdMap();              
    for (iterWpts = stdWaypts->begin(); iterWpts != stdWaypts->end();iterWpts++) {                                     
      if ((*iterWpts).second->ele > 20) {                   
        resultsetWpts.push_back((*iterWpts).second);        
        //   std::cout << "Inserted point with elevation: "
        //      << (*iterWpts).second->ele << std::endl;    
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
