/* Base class for any type of algorithm that generates
 * the macro command file based on the current site*/

/* Basically I did this if anyone ever gets a brighter idea
 * of how to do this, or better yet reimplement quickfort's
 * approach since it seems cool as hell*/

#ifndef MACROALGORITHM_H
#define MACROALGORITHM_H

#include <QObject>

class MacroAlgorithm : public QObject
{
    Q_OBJECT
public:
    MacroAlgorithm();

    virtual void generateMacro() = 0;
};

#endif // MACROALGORITHM_H
