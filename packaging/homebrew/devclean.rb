class Devclean < Formula
  desc "Developer cache cleaner"
  homepage "https://github.com/ChinmayBansal010/devclean"
  url "https://github.com/ChinmayBansal010/devclean/archive/refs/tags/v1.0.1.tar.gz"
  sha256 "REPLACE_WITH_RELEASE_TARBALL_SHA256"
  license "MIT"

  depends_on "cmake" => :build
  depends_on "nlohmann-json"

  def install
    system "cmake", "-S", ".", "-B", "build", "-DCMAKE_BUILD_TYPE=Release"
    system "cmake", "--build", "build"
    system "cmake", "--install", "build", "--prefix", prefix
  end

  test do
    assert_match "devclean v1.0.1", shell_output("#{bin}/devclean version")
  end
end
