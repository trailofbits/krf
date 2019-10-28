# frozen_string_literal: true

Vagrant.configure("2") do |config|
  config.vm.provider :virtualbox do |vb|
    vb.memory = ENV["KRF_VAGRANT_RAM"] || 2048
    vb.cpus = ENV["KRF_VAGRANT_CPUS"] || 2
  end

  config.vm.define "linux" do |linux|
    linux.vm.box = "ubuntu/bionic64"
    linux.vm.provision :shell, inline: <<~PROVISION
      sudo apt update
      sudo DEBIAN_FRONTEND=noninteractive apt upgrade -y
      sudo DEBIAN_FRONTEND=noninteractive apt install -y libelf-dev build-essential ruby linux-headers-$(uname -r)
      sudo apt autoremove apport apport-systems
      echo "/tmp/core_%e.krf.%p" | sudo tee /proc/sys/kernel/core_pattern
    PROVISION

    linux.vm.provider :virtualbox do |vb|
      vb.customize ["modifyvm", :id, "--uartmode1", "disconnected"]
    end
  end

  config.vm.define "freebsd" do |freebsd|
    freebsd.ssh.shell = "sh"

    freebsd.vm.synced_folder ".", "/vagrant", type: :rsync
    freebsd.vm.box = "freebsd/FreeBSD-12.0-RELEASE"
    freebsd.vm.provision :shell, inline: <<~PROVISION
      su -m root -c 'pkg install -y gmake ruby'
      su -m root -c 'svnlite co svn://svn.freebsd.org/base/releng/12.0 /usr/src'
    PROVISION

    freebsd.vm.provider :virtualbox do |vb|
      vb.customize ["modifyvm", :id, "--nictype1", "virtio"]
      vb.customize ["modifyvm", :id, "--nictype2", "virtio"]
    end
  end
end
