#include <QApplication>

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QDebug>

#include <boost/process.hpp>
#include <boost/asio.hpp>
#include <boost/process/windows.hpp>
#include <string>
#include <iostream>
#include <sstream>

#include <QProcess>
#include <QMessageBox>

std::string gCmd = "C:\\Windows\\System32\\cmd.exe";
std::vector< std::string > gArgs = { "/C", "dir", "C:" };

void blaunch( bool showConsole )
{
    boost::process::ipstream outStream;
    boost::process::ipstream errStream;

    std::unique_ptr< boost::process::child > child;
    if ( showConsole )
        child = std::make_unique< boost::process::child >( gCmd, gArgs, boost::process::std_out > outStream, boost::process::std_err > errStream, boost::process::windows::show_normal );
    else
        child = std::make_unique< boost::process::child >( gCmd, gArgs, boost::process::std_out > outStream, boost::process::std_err > errStream, boost::process::windows::hide );

    std::string line;

    std::string out;
    while( std::getline( outStream, line ) )
    {
        if ( !out.empty() )
            out += "\\n";
        out += line;
    }

    std::string err;
    while ( std::getline( errStream, line ) )
    {
        if ( !err.empty() )
            err += "\\n";
        err += line;
    }
    child->wait();

    QMessageBox::information( nullptr, "StdOut", QString::fromStdString( out ) );
    QMessageBox::information( nullptr, "StdErr", QString::fromStdString( err ) );
}

void qlaunch( )
{
    QProcess process;

    QString myCmd = QString::fromStdString( gCmd );
    QStringList args;
    for( auto && ii : gArgs )
        args << QString::fromStdString( ii );
    //QString cmd = "C:/Windows/System32/cmd.exe";
    //auto args = QStringList() << "/C" << "dir" << "C:";
    //auto args = QStringList() << "/C" << "pause";
    process.start( myCmd, args );
    while( !process.waitForFinished( 10 ) )
    {
    }

    auto out = process.readAllStandardOutput();
    auto err = process.readAllStandardError();

    QMessageBox::information( nullptr, "StdOut", out );
    QMessageBox::information( nullptr, "StdErr", err );
}

int main( int argc, char ** argv )
{
    QApplication appl( argc, argv );

    auto * wnd = new QWidget;
    auto pb1 = new QPushButton( "boost::process showConsole" );
    auto pb2 = new QPushButton( "boost::process hide" );
    auto pb3 = new QPushButton( "QProcess" );

    auto layout = new QVBoxLayout;
    layout->addWidget( pb1 );
    layout->addWidget( pb2 );
    layout->addWidget( pb3 );
    wnd->setLayout( layout );

    wnd->show();
    QObject::connect( pb1, &QPushButton::clicked, [](){ blaunch( true ); } );
    QObject::connect( pb2, &QPushButton::clicked, []() { blaunch( false ); } );
    QObject::connect( pb3, &QPushButton::clicked, []() { qlaunch(); } );
    return appl.exec();
}

