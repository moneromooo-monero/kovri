#include <thread>

#include "Daemon.h"

#include "version.h"
#include "transport/Transports.h"
#include "transport/NTCPSession.h"
#include "RouterInfo.h"
#include "RouterContext.h"
#include "tunnel/Tunnel.h"
#include "NetworkDatabase.h"
#include "Garlic.h"
#include "util/util.h"
#include "Streaming.h"
#include "Destination.h"
#include "HTTPServer.h"
#include "ClientContext.h"

namespace i2p
{
    namespace util
    {
        class Daemon_Singleton::Daemon_Singleton_Private
        {
        public:
            Daemon_Singleton_Private() : httpServer(nullptr)
            {};
            ~Daemon_Singleton_Private() 
            {
                delete httpServer;
            };

            i2p::util::HTTPServer *httpServer;
        };

        Daemon_Singleton::Daemon_Singleton() : running(1), d(*new Daemon_Singleton_Private()) , log(kovri::log::Log::Get()) {};
        Daemon_Singleton::~Daemon_Singleton() {
            delete &d;
        };

        bool Daemon_Singleton::IsService () const
        {
#ifndef _WIN32
            return i2p::util::config::GetArg("-service", 0);
#else
            return false;
#endif
        }

        bool Daemon_Singleton::init(int argc, char* argv[])
        {
            i2p::util::config::OptionParser(argc, argv);
            i2p::context.Init ();

            LogPrint("\n\n\n\nkovri starting\n");
            LogPrint("Version ", KOVRI_VERSION);
            LogPrint("data directory: ", i2p::util::filesystem::GetDataDir().string());
            i2p::util::filesystem::ReadConfigFile(
                i2p::util::config::mapArgs, i2p::util::config::mapMultiArgs
            );

            if(i2p::util::config::HasArg("-install")) {
                try {
                    i2p::util::filesystem::InstallFiles();
                    LogPrint("Successfully installed all files.");
                } catch(const std::runtime_error& e) {
                    LogPrint(eLogError, "Failed to install: ", e.what());
                    return false;
                }
            }

            isDaemon = i2p::util::config::GetArg("-daemon", 0);
            isLogging = i2p::util::config::GetArg("-log", 1);
            
            int port = i2p::util::config::GetArg("-port", 0);
            if (port)
                i2p::context.UpdatePort (port);                 
            const std::string host = i2p::util::config::GetArg("-host", "");
            // The host option is deprecated, so this was always true.
            // Because of -Waddress, it's now exposed as the hack it was.
            if (&host && host[0])
                i2p::context.UpdateAddress (boost::asio::ip::address::from_string (host));

            i2p::context.SetSupportsV6 (i2p::util::config::GetArg("-v6", 0));
            i2p::context.SetFloodfill (i2p::util::config::GetArg("-floodfill", 0));
            auto bandwidth = i2p::util::config::GetArg("-bandwidth", "");
            if (bandwidth.length () > 0)
            {
                if (bandwidth[0] > 'L')
                    i2p::context.SetHighBandwidth ();
                else
                    i2p::context.SetLowBandwidth ();
            }   

            LogPrint("CMD parameters:");
            for (int i = 0; i < argc; ++i)
                LogPrint(i, "  ", argv[i]);

            return true;
        }
            
        bool Daemon_Singleton::start()
        {
            // initialize log           
            if (isLogging)
            {
                if (isDaemon)
                {
                    std::string logfile_path = IsService () ? "/var/log" : i2p::util::filesystem::GetDataDir().string();
#ifndef _WIN32
                    logfile_path.append("/kovri.log");
#else
                    logfile_path.append("\\kovri.log");
#endif
                    StartLog (logfile_path);
                }
                else
                    StartLog (""); // write to stdout
            }
            
            try {
                d.httpServer = new i2p::util::HTTPServer(
                   i2p::util::config::GetArg("-httpaddress", "127.0.0.1"),
                   i2p::util::config::GetArg("-httpport", 7070));
                d.httpServer->Start();
                LogPrint("HTTP Server started");
                if(i2p::data::netdb.Start())
                {
                    LogPrint("NetDB started");
                }
                else
                {
                    LogPrint("NetDB failed to start");
                    return false;
                }
                i2p::transport::transports.Start();
                LogPrint("Transports started");
                i2p::tunnel::tunnels.Start();
                LogPrint("Tunnels started");
                i2p::client::context.Start ();
                LogPrint("Client started");
            } catch (std::runtime_error & e) {
                LogPrint(eLogError, e.what());
                return false;
            }
            return true;
        }

        bool Daemon_Singleton::stop()
        {
            LogPrint("Shutdown started.");
            i2p::client::context.Stop();
            LogPrint("Client stopped");
            i2p::tunnel::tunnels.Stop();
            LogPrint("Tunnels stopped");
            i2p::transport::transports.Stop();
            LogPrint("Transports stopped");
            i2p::data::netdb.Stop();
            LogPrint("NetDB stopped");
            d.httpServer->Stop();
            LogPrint("HTTP Server stopped");

            StopLog ();

            delete d.httpServer; d.httpServer = nullptr;

            return true;
        }
    }
}
