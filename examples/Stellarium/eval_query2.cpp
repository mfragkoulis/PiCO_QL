/* C++ lines of code: 22
 * x: excluded
 */
void cpp_query2(SolarSystem *s) {                                         
  PlanetP iterP, iterSP;
  float minAxisRotation, spAxisRotation;                    
  bool inserted;                                            
  std::multimap<float, std::pair<float,std::string> > aggregate;
  std::pair<float, std::string> axisName;                   
  std::multimap<float, std::pair<float, std::string> >::reverse_iterator aggrRit;
  for (int i = 0; i < 100; i++) {                           /*  x */
    minAxisRotation = 100000000;                            /*  x */
    inserted = false;                                       /*  x */
    foreach (iterP,  s->getAllPlanets()) {                              
      inserted = false;                                     
      minAxisRotation = 100000000;                          
      if (StelApp::getInstance().getCore()->getProjection(StelApp::getInstance().getCore()->getHeliocentricEclipticModelViewTransform())->checkInViewport(iterP->screenPos) {
        foreach (iterSP, (*iterP).satellites()) {
          spAxisRotation = (*iterSP).axisRotation;    
          if ((*iterP).axisRotation > spAxisRotation) {
            if (spAxisRotation < minAxisRotation) {           
              minAxisRotation = spAxisRotation;               
              inserted = true;                              /*  x */
            }                                               /*  x */  
          }                                                 /*  x */  
        }                                                   /*  x */
        if (inserted) {
	  axisName=make_pair(minAxisRotation, (*iterP).getNameI18n().toStdString());
          aggregate.insert(std::pair<float, std::pair<float, std::string>>((*iterP).axisRotation, axisName));   
        }                                                   /*  x */  
      }                                                     /*  x */
    }                                                       /*  x */
    std::cout << " name | PlanetRotation | "                
              << "MinSatelliteRotation " << std::endl;      /*  x */
    for (aggrRit = aggregate.rbegin(); aggrRit != aggregate.rend(); aggrRit++) {
      std::cout << (*aggrRit).second.second << " | "        
                << (*aggrRit).first << " | "                /*  x */
                << (*aggrRit).second.first                  /*  x */
                << std::endl;                               /*  x */
    }                                                       /*  x */
    aggregate.clear();                                      /*  x */
  }                                                         /*  x */
}                                                           /*  x */
