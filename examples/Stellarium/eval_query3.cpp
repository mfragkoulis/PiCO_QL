/* C++ lines of code: 20
 * x: excluded
 */
void cpp_query3(SolarSystem *s) {                                         
  PlanetP iterP, iterSP;
  std::multimap<std::string, std::pair<int,Planet *> > aggregate;
  std::pair<int, Planet *> pSatellites;                     
  std::multimap<std::string, std::pair<int, Planet *> >::iterator aggrIt;
  int count = 0;                                            
  Planet *currentP;                                         
  for (int i = 0; i < 100; i++) {                       /*  x */  
    foreach (iterP, s->getAllPlanets()) {                              
      count = 0;                                            
      if (StelApp::getInstance().getCore()->getProjection(StelApp::getInstance().getCore()->getHeliocentricEclipticModelViewTransform())->checkInViewport(iterP->screenPos) {
        foreach (iterSP, (*iterP).satellites()) {
        if ((*iterSP).hasAtmosphere())              
            count++;                                          
        }                                               /*  x */  
        if (count > 0) {                                      
          pSatellites = std::make_pair (count, iterP.data());
          aggregate.insert(std::pair<std::string, std::pair<int, Planet*>>((*iterP).getNameI18n().toStdString(), pSatellites));
        }                                               /*  x */  
      }                                                 /*  x */      
    }                                                   /*  x */      
    std::cout << " name | radius | "                        
              << "period | albedo | NoSatellitesAtm "   /*  x */      
              << std::endl;                             /*  x */      
    for (aggrIt = aggregate.begin(); aggrIt != aggregate.end(); aggrIt++) {
      currentP = (*aggrIt).second.second;                   
      std::cout << (*aggrIt).first << " | "                 
                << currentP->getRadius() << " | "       /*  x */      
                << currentP->getSiderealDay() << " | "  /*  x */      
                << currentP->albedo << " | "            /*  x */      
                << (*aggrIt).second.first               /*  x */      
                << std::endl;                           /*  x */      
    }                                                   /*  x */      
    aggregate.clear();                                  /*  x */      
  }                                                     /*  x */      
}                                                       /*  x */  
