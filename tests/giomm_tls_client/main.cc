#include <giomm.h>
#include <iostream>
#include <cstdlib>

bool on_accept_certificate(const Glib::RefPtr<const Gio::TlsCertificate>& cert, Gio::TlsCertificateFlags)
{
  std::cout << "Handshake is ocurring." << std::endl
    << "The server is requesting that its certificate be accepted." <<
    std::endl;

  std::cout << "Outputing certificate data:" << std::endl <<
    cert->property_certificate_pem().get_value();

  Glib::RefPtr<const Gio::TlsCertificate> issuer = cert->get_issuer();

  std::cout << "Outputing the issuer's certificate data:" << std::endl <<
    issuer->property_certificate_pem().get_value();

  std::cout << "Accepting the certificate (completing the handshake)." <<
    std::endl;

  return true;
}

int main(int, char**)
{
  Gio::init();

  const Glib::ustring test_host = "www.google.com";

  std::vector< Glib::RefPtr<Gio::InetAddress> > inet_addresses =
    Gio::Resolver::get_default()->lookup_by_name(test_host);

  if(inet_addresses.size() == 0)
  {
    std::cout << "Could not resolve test host '" << test_host << "'." <<
      std::endl;
    return EXIT_FAILURE;
  }

  std::cout << "Successfully resolved address of test host '" << test_host <<
    "'." << std::endl;

  Glib::RefPtr<Gio::InetAddress> first_inet_address = inet_addresses[0];

  std::cout << "First address of test host is " <<
    first_inet_address->to_string() << "." << std::endl;

  Glib::RefPtr<Gio::Socket> socket =
    Gio::Socket::create(Gio::SOCKET_FAMILY_IPV4, Gio::SOCKET_TYPE_STREAM,
    Gio::SOCKET_PROTOCOL_TCP);

  Glib::RefPtr<Gio::InetSocketAddress> address =
    Gio::InetSocketAddress::create(first_inet_address, 443);

  socket->connect(address);

  if(!socket->is_connected())
  {
    std::cout << "Could not connect socket to " <<
      address->get_address()->to_string() << ":" << address->get_port() <<
      "." << std::endl;
  }

  Glib::RefPtr<Gio::TcpConnection> conn = Glib::RefPtr<Gio::TcpConnection>::cast_dynamic(Gio::SocketConnection::create(socket));

  if(!conn || !conn->is_connected())
  {
    std::cout << "Could not establish connection to " <<
      address->get_address()->to_string() << ":" << address->get_port() <<
      "." << std::endl;
    socket->close();
    return EXIT_FAILURE;
  }

  std::cout << "Successfully established connection to " <<
    address->get_address()->to_string() << ":" << address->get_port() <<
    "." << std::endl;

  Glib::RefPtr<Gio::TlsClientConnection> tls_connection;

  try
  {
    Glib::RefPtr<Gio::TlsClientConnection> tls_connection =
      Gio::TlsClientConnection::create(conn, address);

    tls_connection->signal_accept_certificate().connect(
      sigc::ptr_fun(&on_accept_certificate));

    tls_connection->handshake(); 

    std::cout << "Attempting to get the issuer's certificate from the "
      "connection." << std::endl;

    Glib::RefPtr<Gio::TlsCertificate> issuer_certificate =
      tls_connection->get_peer_certificate()->get_issuer();

    if(!issuer_certificate)
    {
      std::cout << "Could not get the issuer's certificate of the peer." <<
        std::endl;
      return EXIT_FAILURE;
    }
    std::cout << "Successfully retrieved the issuer's certificate." <<
      std::endl;

    std::cout << "Attempting to use the connection's database." << std::endl;
    Glib::RefPtr<Gio::TlsDatabase> database = tls_connection->get_database();

    std::cout << "Looking up the certificate's issuer in the database." <<
      std::endl;

    Glib::RefPtr<Gio::TlsCertificate> db_certificate =
      database->lookup_certificate_issuer(issuer_certificate);

    if(!db_certificate)
    {
      std::cout << "The issuer's certificate was not found in the database." << std::endl;
    }
    else
    {
      std::cout << "Successfully found the issuer's certificate in the database." << std::endl;
    }
  }
  catch (const Gio::TlsError& error)
  {
    std::cout << "Exception caught: " << error.what() << "." << std::endl;
    return EXIT_FAILURE;
  }

  conn->close();

  return EXIT_SUCCESS;
}
