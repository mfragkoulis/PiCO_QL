void cpp_query2() {                                         
  clock_t start, finish;                                    
  double t = 0;                                             
  PlanetP iterP, iterSP;
  float minAxisRotation, spAxisRotation;                    
  bool inserted;                                            
  std::multimap<float, std::pair<float,std::string> > aggregate;
  std::pair<float, std::string> axisName;                   
  std::multimap<float, std::pair<float, std::string> >::reverse_iterator aggrRit;
  for (int i = 0; i < 100; i++) {                           
    start = clock();                                        
    minAxisRotation = 100000000;                            
    inserted = false;                                       
    foreach (iterP,  s->getAllPlanets()) {                              
      inserted = false;                                     
      minAxisRotation = 100000000;                          
      if (StelApp::getInstance().getCore()->getProjection(StelApp::getInstance().getCore()->getHeliocentricEclipticModelViewTransform())->checkInViewport(iterP->screenPos) {
        foreach (iterSP, (*iterP).satellites()) {
          spAxisRotation = (*iterSP).axisRotation;    
          if ((*iterP).axisRotation > spAxisRotation) {
            if (spAxisRotation < minAxisRotation) {           
              minAxisRotation = spAxisRotation;               
              inserted = true;                                
            }                                                 
          }                                                   
        }
        if (inserted) {
	  axisName=make_pair(minAxisRotation, (*iterP).getNameI18n().toStdString());
          aggregate.insert(std::pair<float, std::pair<float, std::string>>((*iterP).axisRotation, axisName));   
        }                                                     
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
