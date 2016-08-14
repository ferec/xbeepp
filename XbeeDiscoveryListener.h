#ifndef XBEEDISCOVERYLISTENER_H
#define XBEEDISCOVERYLISTENER_H


class XbeeDiscoveryListener
{
    public:

        virtual void xbeeDiscovered(XbeeRemote *remote) = 0;

        void listensTo(XbeeLocal *local) { listens = local; }
    protected:
    private:
        XbeeLocal *listens;
};

#endif // XBEEDISCOVERYLISTENER_H
