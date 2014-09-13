---
layout : post
title : QDBus
categories : [programming, Qt]
---
Often I want to read a PDF while cooking dinner. The projector is visible from the stove, but the desk is to far away for convenient scrolling. Thankfully, most KDE apps (including Okular) implement a IPC interface using qdbus. I realized that with a command line interface to Okular, my phone could (via SSH) be used as a remote control.

This turned out to be quite straightforward. The `qdbus` command (with no arguments) provides a list of all applications that have registered with it. By filtering for `'okular'`, we can find get an interface to all running Okular instances.

Okular implements a `currentDocument` method, so we can find which instance has the relevant document. We can then send any number of signals, most usefully `slotNextPage` and `slotPreviousPage`

To read a document, I then open it regularly, SSH to my desktop, and send `okular_ctl filename slotNextPage` whenever I want to see the next page.

{% highlight bash %}
#!/bin/sh

function print_help {
        echo "${0} DOCUMENT_NAME ACTION"
        echo DOCUMENT_NAME should be a pattern that matches the name of any documents to control
        echo ACTION is one of: currentPage goToPage pages
        echo "                  slotGotoFirst slotGotoLast slotNextPage"
        echo "                  slotPreviousPage slotTogglePresentation"
}

if [[ ${1} = "--help" ]] ; then print_help ; exit 0 ; fi
document_name=${1}
echo -n "Attempting to find ${document_name}"
action=${2}
echo " to do ${action}"

echo -n "Finding running instances of okular... "
ifaces=$(qdbus | grep "okular")
iface_count=$(echo ${ifaces} | wc -w /dev/stdin | awk '{print $1}')
echo "Found ${iface_count}"

for iface in ${ifaces} ; do
        echo -n "Getting name on ${iface}... "
        current_document=$(qdbus ${iface} /okular org.kde.okular.currentDocument)
        echo "${current_document}"
        echo ${current_document} | grep --color=ALWAYS ${document_name}
        matches=$?
        if [[ ${matches} -eq 0 ]] ; then
                qdbus ${iface} /okular org.kde.okular.${action}
        fi;
done
{% endhighlight %}
