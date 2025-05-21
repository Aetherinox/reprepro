<div align="center">
<h6>Debian / Ubuntu package manager</h6>
<h1>📦 Reprepro 📦</h1>
</div>

**Reprepro** is a tool for managing APT repositories. Reprepro is able to manage multiple repositories for multiple distribution versions and one package pool.

<br />

---

<br />

- [Install](#install)
  - [From Github](#from-github)
  - [From Proteus Apt Repo](#from-proteus-apt-repo)
- [Uninstall](#uninstall)
- [Build](#build)
- [Create Personal Repo](#create-personal-repo)
  - [Step 1: Create GPG Key](#step-1-create-gpg-key)
    - [Certify Key](#certify-key)
    - [Sign Key](#sign-key)
  - [Step 2: Create Github Repo](#step-2-create-github-repo)
  - [Step 3: Publish First Package](#step-3-publish-first-package)
  - [Step 4: Use Your New Repo](#step-4-use-your-new-repo)

<br />

---

<br />

## Install

There are a few ways you can install the latest version of Reprepro:

<br />

### From Github

Run the following command to download the `.deb` package:

```shell
wget https://github.com/Aetherinox/reprepro/releases/download/5.4.7-1-debian/reprepro_5.4.7-1_amd64.deb
```

<br />

Then install the package with:

```shell
sudo dpkg -i reprepro_5.4.7-1_amd64.deb
```

<br />

### From Proteus Apt Repo

We host our own Debian repository that you can download the package from. Open `Terminal` and add the GPG key to your keyring. _(pick one)_:

```shell
# Using curl
curl -fsSL https://github.com/Aetherinox.gpg | \
  sudo gpg --batch --yes --dearmor -o /usr/share/keyrings/aetherinox-proteus-archive.gpg

# Using wget
wget -qO - https://github.com/Aetherinox.gpg | \
  sudo gpg --batch --yes --dearmor -o /usr/share/keyrings/aetherinox-proteus-archive.gpg
```

<br />

Fetch the repo package list with one of the following commands. The command **with variables** will download the package based on your current machine architecture and debian distro. The command without variables allows you to specify which package you can download. _(pick one)_:

```shell
# Using variables
echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/aetherinox-proteus-archive.gpg] https://raw.githubusercontent.com/Aetherinox/proteus-apt-repo/main $(lsb_release -cs) main" | \
  sudo tee /etc/apt/sources.list.d/aetherinox-proteus-archive.list

# Without variables
echo "deb [arch=amd64 signed-by=/usr/share/keyrings/aetherinox-proteus-archive.gpg] https://raw.githubusercontent.com/Aetherinox/proteus-apt-repo/main focal main" | \
  sudo tee /etc/apt/sources.list.d/aetherinox-proteus-archive.list
```

<br />

> [!note]
> **Optional** Verify Package
>
> If you want to verify that you downloaded the correct package, run the following command in terminal:
> 
> ```shell
> gpg -n -q --import --import-options import-show \
>     /usr/share/keyrings/aetherinox-proteus-archive.gpg | \
>     awk -v val=$GPG_KEY_FINGERPRINT '/pub/{getline; gsub(/^ +| +$/,"");
>     if($0 == val)
>         print "("$0") Found match for Proteus Apt Repo."
>     else
>         print "("$0") No match for Proteus Apt Repo."}'
> ```
>
> You should see the following output:
> ```shell
> (BCA07641EE3FCD7BC5585281488D518ABD3DC629) Found match for Proteus Apt Repo.
> ```
> 

<br />

You can now install Reprepro and Lintian with the following commands:

```shell
sudo apt update
sudo apt install reprepro
```

<br />

You can confirm Reprepro is installed by running the command:

```shell
$ reprepro --version
reprepro: This is reprepro version 5.4.7
```

<br />

---

<br />

## Uninstall

To remove Reprepro from your Linux system; run:

```shell
sudo apt-get remove reprepro
sudo apt-get purge --auto-remove reprepro
sudo apt-get clean
```


<br />

---

<br />

## Build

To build the Reprepro package; open your `/etc/apt/sources.list` and uncomment any line containing `deb-src` at the beginning.

```console
deb http://us.archive.ubuntu.com/ubuntu/ jammy main restricted
deb-src http://us.archive.ubuntu.com/ubuntu/ jammy main restricted

deb http://us.archive.ubuntu.com/ubuntu/ jammy-updates main restricted
deb-src http://us.archive.ubuntu.com/ubuntu/ jammy-updates main restricted

[...]
```

<br />

Save the file and then update your machine's package sources by running the command:

```shell
apt update
```

<br />

Install dev tools

```shell
apt install build-essential fakeroot devscripts git zstd
```

<br />

Clone the latest code

```shell
mkdir reprepro && cd reprepro
git clone --depth=1 -b reprepro-debian-5.4.7-1 https://github.com/Aetherinox/reprepro.git .
```

<br />

Install build dependencies for reprepro

```shell
apt build-dep .
```

<br />


Now build Reprepro

```shell
debuild -b -uc -us 
```

<br />

Once built, the `.deb` packages will be one folder back; navigate by typing:

```shell
cd ..
```

<br />

You can now install the package with one of two commands:

```shell
# Option 1
sudo apt install reprepro_5.4.7-1_amd64.deb

# Option 2
sudo dpkg -i reprepro_5.4.7-1_amd64.deb
```

<br />

If you need to transfer the `.deb` package to another machine; you have a few options such as:

1. Use [transfer.sh](https://github.com/dutchcoders/transfer.sh)
2. Upload to personal apt repository

<br />

For [transfer.sh](https://github.com/dutchcoders/transfer.sh), run the command:

```shell
curl \
  --upload-file reprepro_5.4.7-1_amd64.deb \
  https://transfer.domain.lan/reprepro_5.4.7-1_amd64.deb
```

<br />

For using a personal apt repository, you will need to set this up yourself. You can use either an nginx / apache2 webserver; or you can host your Linux / debian packages on Github and then add that Github repo to your Linux machine's trusted sources list at `/etc/apt/sources.list`.

<br />

---

<br />

## Create Personal Repo

This section explains how you can use Reprepro + Github to create your own Linux package repository.

<br />

### Step 1: Create GPG Key

Before we can create our new Github repository; you will need to generate a GPG key. This key is what you will use to sign your packages with. If your repository and packages are not signed, Linux will throw a fit.

<br />

There are four subkeys that make up an entire GPG pair, each type is listed below:

- `[C]ertify`
- `[S]ign`
- `[A]uthenticate`
- `[E]ncrypt`

<br />

The only key types we need for this to work is a `[C]ertify` and `[S]ign` subkey. You can generate all four if you want, but `C` and `S` are the only required keys needed.

<br />

If you haven't installed GPG, run the command:

```shell
sudo apt update
sudo apt install gpg
```

<br />
<br />

#### Certify Key

First, create our `[C]` key with the command:

```shell
gpg --expert --full-generate-key
```

<br />

When the text appears, select the following:

- `(8)` **RSA (set your own capabilities)**
  - `(E)` Toggle the encrypt capability
  - `(S)` Toggle the sign capability
  - `(Q)` Finished

<br />

When asked questions, use the following:

<br />

> [!note]
> When asked for your **email address**; it is highly recommended that you set the email address to the same email used for your Github account. Otherwise your GPG key will not be verified.

<br />

- Key Size: `4096`
- Expires: `0`
- Real name: `Your Name`
- Email: `your@email.com` (_see note above_)
- Comment: `RSA 4096`

<br />

You will then be asked to confirm your information:

```shell
You selected this USER-ID:
    "Aetherinox (RSA 4096) <my@email.com>"
     
Change (N)ame, (C)omment, (E)mail or (O)kay/(Q)uit? o
```

<br />

Press `O` for Okay. You will be prompted for a password / passphrase either via a dialog box or in the console window. Create a strong passphrase and press OK. GPG will request you to move your mouse:

```
We need to generate a lot of random bytes. It is a good idea to perform
some other action (type on the keyboard, move the mouse, utilize the
disks) during the prime generation; this gives the random number
generator a better chance to gain enough entropy.
```

<br />

Simply move your mouse, open/close windows. Do as many actions on your device as you can while it generates to help with entropy.

<br />

The system will output a path to your revocation certificate. Save this file somewhere. GPG will display your generated key, as you can see in the output; you will now have the `[C]ertify` key:

```properties
pub   rsa4096 2025-06-05 [C]
      1AEC46BD3EC15FCA34C2F2C2468FE17ECF3CFAE4
uid                      Aetherinox (RSA 4096) <my@email.com>
```

<br />
<br />

#### Sign Key

<br />

You now need to create your `[S]ign` key, copy the GPG key you were given for your `[C]ertify` key and run the command:

```shell
gpg --expert --edit-key 1AEC46BD3EC15FCA34C2F2C2468FE17ECF3CFAE4
```

<br />

It will ask you to type a command for what you want to do, type 

```shell
addkey
```

<br />

Since you already know the process, we'll quickly outline the options to select:

- `(4)` **RSA (sign only)**
  - Key Size: `4096`
  - Expires: `0`

<br />

Confirm your changes. You will be prompted to enter the passphrase you selected when you made the `MASTER / [C]ertify` key in the steps above. Enter that passphrase. Move your mouse around to generate entropy.

<br />

We now have a master key`[C]ertify`, and one sub key `[S]ign`

```console
sec  rsa4096/468FE17ECF3CFAE4
     created: 2025-06-05  expires: never       usage: C
     trust: ultimate      validity: ultimate

ssb  rsa4096/52EC15AD56FE4567
     created: 2025-06-05  expires: never       usage: S

[ultimate] (1). Aetherinox (RSA 4096) <my@email.com>
```

<br />

Since we have our `[C]ertify` and `[S]ign` key, this is as far as we need to go. If you want to generate an `[A]uthenticate` and `[E]ncrypt` subkey, you can. We are not going to give the instructions for that. Next, we will export our keys:

<br />

List your current keys:

```shell
gpg --list-secret-keys --keyid-format=short
```

<br />

You should see a list of your keys:

```console
sec   rsa4096/CF3CFAE4 2025-06-05 [C]
      1AEC46BD3EC15FCA34C2F2C2468FE17ECF3CFAE4
uid         [ultimate] Aetherinox (RSA 4096) <my@email.com>
ssb   rsa4096/56FE4567 2025-06-05 [S]
```

<br />

In the list above, copy the `[C]ertify` keys 8 digit ID:
- `CF3CFAE4`

<br />

You need to create a new group of folders to save your keys in; run the command:

```shell
sudo mkdir -p ./keys/{public,private}
```

Now run the commands to export your **Public GPG Key**:

```shell
gpg --output "keys/public/username.pub.gpg" --export CF3CFAE4
gpg --output "keys/public/username.pub.asc" --export --armor CF3CFAE4
```

<br />

Now run the following commands to export your **Private GPG Keys**. When you export these, you will be asked to provide the **passphrase** you set up earlier when you generated your keys:

```shell
gpg --output "keys/private/username.priv.gpg" --export-secret-key CF3CFAE4
gpg --output "keys/private/username.priv.asc" --armor --export-secret-key CF3CFAE4
```

<br />

After running the commands above, you should have four (4) new files:

- `keys/public/username.pub.gpg`
- `keys/public/username.pub.asc`
- `keys/private/username.priv.gpg`
- `keys/private/username.priv.asc`

<br />

The files ending with `gpg` are binary copies of your public and private key. The files ending with `asc` are armored copies of your public and private key. The `.gpg` and `.asc` files are the same keys, just in different formats. Your repository will use the `.asc armored` keys.

<br />

If you need to import your GPG keys onto another system, you can transfer the keys and then use the command:

```shell
gpg --import keys/private/username.priv.asc
```

<br />

Now is a good time to add these newly generated keys to your [Github account](https://github.com). Sign into Github and go to the page: 
- https://github.com/settings/keys

<br />

<p align="center"><img style="width: 80%;text-align: center;" src=".docs/img/create/2.png"></p>




<br />

After you have completed all of this, proceed to the next step [Step 2: Create Github Repo](#step-2-create-github-repo)

<br />
<br />

### Step 2: Create Github Repo

<br />

Next, sign into your Github account and create a new repository.

<p align="center"><img style="width: 80%;text-align: center;" src=".docs/img/create/1.jpg"></p>

<br />

Now visit your new repository, you may want to clone this new repository to your system so that you can create the required folders and commit.

<br />

```shell
mkdir -p github/my-apt-repo
cd github/my-apt-repo
git clone https://github.com/username/my-apt-repo.git .
```

<br />

Create a new folder for your project: 
- `github/my-apt-repo/conf`
- `github/my-apt-repo/incoming`
- `github/my-apt-repo/db`
- `github/my-apt-repo/dists`
- `github/my-apt-repo/pool`

```shell
mkdir -p github/my-apt-repo/{conf,incoming,db,dists,pool}
```

<br />

Create a new file in `github/my-apt-repo/conf/distributions`. You can use the command below, or create it in notepad:

```shell
# Create the distributions file
cat <<EOF > github/my-apt-repo/conf/distributions
    Origin: Aetherinox
    Label: Noble 24.04
    Suite: stable
    Codename: noble
    Architectures: amd64 arm64 i386 source
    Components: main
    Description: Ubuntu 24.04 (Noble) LTS
    SignWith: BD3DC629
EOF
```

<br />

> [!note]
> The **SignWith** property specifies the GPG key you will use to sign your repository with.
>
> You should have your GPG keys already generated if you followed the instructions in the previous section:
>   - [Step 1: Create GPG Key](#step-1-create-gpg-key)
> 

<br />

If you don't want to use the Linux `cat` command above, you can paste the following inside `github/my-apt-repo/conf/distributions`.  The below file is an example of how it is set up. We will add **Ubuntu Noble**:

```ini
Origin: Aetherinox
Label: Noble Numbat 24.04 LTS
Suite: stable
Codename: noble
Architectures: amd64 arm64 i386 source
Components: main
Description: Ubuntu 24.04 (Noble Numbat)
SignWith: BD3DC629
Log: logs/noble.log
Limit: 0
```

<br />

To add more than one, simply add a blank line and start again:

```ini
Origin: Aetherinox
Label: Noble Numbat 24.04 LTS
Suite: stable
Codename: noble
Architectures: amd64 arm64 i386 source
Components: main
Description: Ubuntu 24.04 (Noble Numbat)
SignWith: BD3DC629
Log: logs/noble.log
Limit: 0

Origin: Aetherinox
Label: Jammy Jellyfish 22.04 LTS
Suite: stable
Codename: jammy
Architectures: amd64 arm64 i386 source
Components: main
Description: Ubuntu 22.04 (Jammy Jellyfish)
SignWith: BD3DC629
Log: logs/jammy.log
Limit: 0
```

<br />

> [!note]
> The **Limit** property allows you to define how many versions of the package will be stored in your Debian repo at a time. Set this to `0` for **unlimited**. If you only want the latest version of each package, and to NOT save older versions, set `Limit: 1`
>
> If you set `Limit: 1`, then every time a new version of a package is added, all older copies will be deleted and cleaned from your repo.

<br />

Once you finish the above instructions, you are ready to add your first package. If you did all of the above by cloning your new debian repo, make sure to commit your changes first:

```shell
git add .
git commit -m "chore: initialize debian repo"
git push
```

<br />
<br />

### Step 3: Publish First Package

To add our first package, you need to first find a `.deb` package you want to add to your new debian repo. For this example, we'll download and add Reprepro. Make sure you are in the root folder of your new debian repository; you should see:

```console
📁 my-apt-repo
   📁 conf
   📁 incoming
   📁 db
   📁 dists
   📁 pool
```

<br />

`cd` into the root folder:

```shell
cd my-apt-repo
```

<br />

Now download a test `.deb` package such as Reprepro:

```shell
wget https://github.com/Aetherinox/reprepro/releases/download/5.4.7-1-debian/reprepro_5.4.7-1_amd64.deb
```

<br />

We will add Reprepro to our debian repository, and mark the package as being for the `amd64` architecture, and for Ubuntu distro `noble`. Run the command below; edit it to fit your needs.

<br />

For the architecture, you have the following options:
- `amd64`
- `arm64`
- `i386`
- `all`

<br />

For `all`, skip the command below and use the next one:

<br />

```shell
# add package to amd64

reprepro -V \
    --section utils \
    --component main \
    --priority 0 \
    --architecture amd64 \
    includedeb noble "reprepro_5.4.7-1_amd64.deb"
```

<br />

Once you complete the command, you should see:

```shell
Exporting indices...
Successfully created './dists/noble/Release.gpg.new'
Successfully created './dists/noble/InRelease.new'
```

<br />

If you have a package which you want to add to `all` and is not limited to any one specific aritecture, the command will look slightly different. We will remove the `--architecture` option we used above. This will specify that the package is for any architecture:

```shell
reprepro -V \
    --section utils \
    --component main \ 
    --priority 0 \
    includedeb noble "reprepro_5.4.7-1_all.deb"
```

<br />

Some debian packages will be labeled with the filename `_all.deb` and some will only be for specific architectures. So pay close attention to what architecture a .deb file is generated for.

<br />

Now that we have added a package, push the new package and changes:

```shell
git add .
git commit -m "chore: add package reprepro_5.4.7-1_amd64.deb"
git push
```

<br />
<br />

### Step 4: Use Your New Repo

Now that we have created our own Debian / Ubuntu repository; we can add it to our own system and download packages from it. To do this, we need to add the GPG key you generated in the section [Step 1: Create GPG Keys ](#step-1-create-gpg-key).

<br />

Github stores our public GPG key when we add it to our Github account, it is accessible by going to the URL below (change the USERNAME to your own):
- https://github.com/Username.gpg

<br />

Once you have your public GPG key, paste the URL in the command below:

<br />

```
curl -fsSL https://github.com/Username.gpg | \
  sudo gpg --batch --yes --dearmor -o /usr/share/keyrings/username-my-apt-repo.gpg
```

<br />

Next, add your new repository to your Linux source list. Make sure to:

- Change `USERNAME` to your own username
- Change `my-apt-repo` to the name of your Github repo
- Change `main` to the branch you are using in the Github repo.
  - Sometimes this is also called `master`

```shell
echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/USERNAME-my-apt-repo.gpg] https://raw.githubusercontent.com/USERNAME/my-apt-repo/main $(lsb_release -cs) main" | \
  sudo tee /etc/apt/sources.list.d/USERNAME-my-apt-repo.list
```

<br />

Now update your packages so that the new repo is added:

```shell
sudo apt update
```

<br />

We can actually see where a package is being distributed from by running the command:

```shell
apt-get download --print-uris reprepro
```

<br />

You should see the response which says it's coming from your own Github repository:

```console
'https://raw.githubusercontent.com/USERNAME/my-apt-repo/main/pool/main/r/reprepro/reprepro_5.4.7-1_amd64.deb' reprepro_5.4.7-1_amd64.deb 2310 
SHA512:5d04d99e636b897e39c7b7b834e845cd7bc98ed82e5ebb7657fc035819e05d585ddc3c7a8c6e2c9c3c47e56d4529d7b2b185addceb1c2115b6ea32982d6f6de6
```

<br />

We can now install the latest version of Reprepro from our own Github hosted Debian repository:

```shell
sudo apt install reprepro
```

<br />

This concludes the basics of getting a Github hosted Debian / Ubuntu repository started.

<br />

---

<br />