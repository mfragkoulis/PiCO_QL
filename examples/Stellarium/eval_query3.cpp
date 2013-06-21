void cpp_query3(SolarSystem *s) {                                         
  clock_t start, finish;                                    
  double t = 0;                                             
  PlanetP iterP, iterSP;
  std::multimap<std::string, std::pair<int,Planet *> > aggregate;
  std::pair<int, Planet *> pSatellites;                     
  std::multimap<std::string, std::pair<int, Planet *> >::iterator aggrIt;
  int count = 0;                                            
  Planet *currentP;                                         
  for (int i = 0; i < 100; i++) {                           
    start = clock();                                        
    count = 0;                                              
    foreach (iterP, s->getAllPlanets()) {                              
      count = 0;                                            
      if (StelApp::getInstance().getCore()->getProjection(StelApp::getInstance().getCore()->getHeliocentricEclipticModelViewTransform())->checkInViewport(iterP->screenPos) {
        foreach (iterSP, (*iterP).satellites()) {
        if ((*iterSP).hasAtmosphere())              
            count++;                                          
        }
        if (count > 0) {                                      
          pSatellites = std::make_pair (count, iterP.data());
          aggregate.insert(std::pair<std::string, std::pair<int, Planet*>>((*iterP).getNameI18n().toStdString(), pSatellites));
        }
      }                                                     
    }                                                       
    std::cout << " name | radius | "                        
              << "period | albedo | NoSatellitesAtm "       
              << std::endl;                                 
    for (aggrIt = aggregate.begin(); aggrIt != aggregate.end(); aggrIt++) {
      currentP = (*aggrIt).second.second;                   
      std::cout << (*aggrIt).first << " | "                 
                << currentP->getRadius() << " | "           
                << currentP->getSiderealDay() << " | "      
                << currentP->albedo << " | "                
                << (*aggrIt).second.first                   
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
