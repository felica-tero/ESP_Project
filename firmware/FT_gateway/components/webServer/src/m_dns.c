// #include "m_dns.h"
// #include "mdns.h"

// void start_mdns_service()
// {
//     //initialize mDNS service
//     esp_err_t err = mdns_init();
//     if (err) {
//         printf("MDNS Init failed: %d\n", err);
//         return;
//     }

//     //set hostname
//     mdns_hostname_set("my-esp32");
//     //set default instance
//     mdns_instance_name_set("Jhon's ESP32 Thing");
// }

// void add_mdns_services()
// {
//     //add our services
//     mdns_service_add(NULL, "_http", "_tcp", 80, NULL, 0);
//     mdns_service_add(NULL, "_arduino", "_tcp", 3232, NULL, 0);
//     mdns_service_add(NULL, "_myservice", "_udp", 1234, NULL, 0);

//     //NOTE: services must be added before their properties can be set
//     //use custom instance for the web server
//     mdns_service_instance_name_set("_http", "_tcp", "Jhon's ESP32 Web Server");

//     mdns_txt_item_t serviceTxtData[3] = {
//         {"board","{esp32}"},
//         {"u","user"},
//         {"p","password"}
//     };
//     //set txt data for service (will free and replace current data)
//     mdns_service_txt_set("_http", "_tcp", serviceTxtData, 3);

//     //change service port
//     mdns_service_port_set("_myservice", "_udp", 4321);
// }

// void resolve_mdns_host(const char * host_name)
// {
//     printf("Query A: %s.local", host_name);

//     struct ip4_addr addr;
//     addr.addr = 0;

//     esp_err_t err = mdns_query_a(host_name, 2000,  &addr);
//     if(err){
//         if(err == ESP_ERR_NOT_FOUND){
//             printf("Host was not found!");
//             return;
//         }
//         printf("Query Failed");
//         return;
//     }

//     printf(IPSTR, IP2STR(&addr));
// }