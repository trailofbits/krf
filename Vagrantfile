# frozen_string_literal: true

PROVISION = <<~SCRIPT
  sudo apt update
  sudo apt upgrade -y
  sudo apt install -y libelf-dev build-essential linux-headers-$(uname -r)
  sudo useradd -m krf
SCRIPT

Vagrant.configure("2") do |config|
  config.vm.box = "ubuntu/bionic64"
  config.vm.provision "shell", inline: PROVISION

  config.vm.provider "virtualbox" do |vb|
    vb.customize ["modifyvm", :id, "--uartmode1", "disconnected"]
  end
end
