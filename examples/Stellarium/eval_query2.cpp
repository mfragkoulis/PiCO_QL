void cpp_query2() {                                         
  clock_t start, finish;                                    
  double t = 0;                                             
  std::list<QSharedPointer<Planet> >::iterator iterP, iterSP;
  float minAxisRotation, spAxisRotation;                    
  bool inserted;                                            
  std::multimap<float, std::pair<float,std::string> > aggregate;
  std::pair<float, std::string> axisName;                   
  std::multimap<float, std::pair<float, std::string> >::reverse_iterator aggrRit;
  for (int i = 0; i < 100; i++) {                           
    start = clock();                                        
    minAxisRotation = 100000000;                            
    inserted = false;                                       
    for (iterP = listSystemPlanets->begin(); iterP != listSystemPlanets->end();iterP++) {                              
      inserted = false;                                     
      minAxisRotation = 100000000;                          
      for (iterSP=(*iterP).data()->getStdSatellites()->begin();
        iterSP != (*iterP).data()->getStdSatellites()->end(); iterSP++) {
        spAxisRotation = (*iterSP).data()->axisRotation;    
        if ((*iterP).data()->axisRotation > spAxisRotation) {
          if (spAxisRotation < minAxisRotation) {           
            minAxisRotation = spAxisRotation;               
            inserted = true;                                
          }                                                 
        }                                                   
      }                                                     
      if (inserted) {
	axisName=make_pair(minAxisRotation, (*iterP).data()->getNameI18n().toStdString());
        aggregate.insert(std::pair<float, std::pair<float, std::string>>((*iterP).data()->axisRotation, axisName));   
      }                                                     
    }                                                       
    std::cout << " name | PlanetRotation | "                
              << "MinSatelliteRotation " << std::endl;      
    for (aggrRit = aggregate.rbegin(); aggrRit != aggregate.rend(); aggrRit++) {                                       
      std::cout << (*aggrRit).second.second << " | "        
                << (*aggrRit).first << " | "                
                << (*aggrRit).second.first                  
                << std::endl;                               
    }                                                       
    aggregate.clear();                                      
    finish = clock();                                       
    t += ((double)finish - (double)start)/CLOCKS_PER_SEC;   
  }                                                         
  t = t/100;                                                
  std::cout << "Query took " << t                           
            << " seconds to execute."                       
            << std::endl;                                   
}                                                           
