#include "BvSystem.h"

/*!
 * \brief main launches the application- entry point.  Instantiates BvSystem, which starts the application.
 *
 * \param argc default launch parameters.
 * \param argv default launch parameters.
 *
 * \return the execution of BvSystem (it inherits from QApplication)
 */
int main(int argc, char *argv[])
{
    // Instantiates ThreadManager and WindowManager (WindowManager builds and shows MainWindow in the constructor)
    BvSystem system (argc, argv);

    return system.exec();
}
