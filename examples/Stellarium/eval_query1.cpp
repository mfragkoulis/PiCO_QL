void cpp_query1() {                                         
  clock_t start, finish;                                    
  double t = 0;                                             
  std::list<PlanetP>::iterator iter;                        
  std::set<double> resultsetP;                              
  double min_distance;                                      
  std::vector<Meteor*>* stdMet;                             
  std::vector<Meteor*>::iterator it;                        
  for (int i = 0; i< 100; i++) {                            
    start = clock();                                        
    *stdSystemPlanets = stdSolar->getAllPlanets().toStdList();
    for (iter = stdSystemPlanets->begin(); iter != stdSystemPlanets->end(); iter++) {                                  
      if (strcmp((*iter).data()->getNameI18n().toStdString().c_str(), "Earth"))                                        
        resultsetP.insert((*iter).data()->getDistance());   
    }                                                       
    min_distance = *min_element(resultsetP.begin(), resultsetP.end());                                                 
    stdMet = stdMeteors->getActive();                       
    if (stdMet->size() > 0) {                               
      std::cout << "Min distance is:: " << min_distance   
                << std::endl;                               
      std::cout << "observdistance | velocity | "           
                << " magnitude | scalemagnitude "           
                << std::endl;                               
    }                                                       
    for (it = stdMet->begin(); it != stdMet->end(); it++) {
      if (((*it)->xydistance > min_distance) && ((*it)->alive)) {                                                      
        std::cout << (*it)->xydistance << " | "             
                  << (*it)->velocity << " | "               
                  << (*it)->mag << " | "                    
                  << (*it)->distMultiplier                  
                  << std::endl;                             
      }                                                     
    }                                                       
    finish = clock();                                       
    t += ((double)finish - (double)start)/CLOCKS_PER_SEC;   
  }                                                         
  if (stdMet->size() > 0) {                                 
      std::cout << "Query took " << t/100                   
                << " seconds to execute."                   
                << std::endl;                               
  }                                    
}
