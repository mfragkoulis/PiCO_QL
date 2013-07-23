/* C++ lines of code: 14
 * x: excluded
 */
void cpp_query1() {
  PlanetP iter;
  std::set<double> resultsetP;
  double min_distance;
  std::vector<Meteor*>::iterator it;
  for (int i = 0; i< 100; i++) {                      /*  x */
    foreach (iter,  solar->getAllPlanets()) {
      if (strcmp(iter.data()->getNameI18n().toStdString().c_str(), "Earth"))
        resultsetP.insert(iter.data()->getDistance());
    }                                                 /*  x */
    min_distance = *min_element(resultsetP.begin(), resultsetP.end());
    if (meteor->getActive() && meteor->getActive()->size() > 0) {
      std::cout << "Min distance is:: " << min_distance
                << std::endl;                         /*  x */
      std::cout << "observdistance | velocity | "     /*  x */
                << " magnitude | scalemagnitude "     /*  x */
                << std::endl;                         /*  x */
      for (it = meteor->getActive()->begin(); it != meteor->getActive()->end(); it++) {
        if (((*it)->xydistance > min_distance) && ((*it)->alive)) {
          std::cout << (*it)->xydistance << " | "
                    << (*it)->velocity << " | "       /*  x */
                    << (*it)->mag << " | "            /*  x */
                    << (*it)->distMultiplier          /*  x */
                    << std::endl;                     /*  x */
        }                                             /*  x */
      }                                               /*  x */
    }                                                 /*  x */
  }                                                   /*  x */
}                                                     /*  x */

