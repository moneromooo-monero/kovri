/**
 * Copyright (c) 2013-2016, The Kovri I2P Router Project
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be
 *    used to endorse or promote products derived from this software without specific
 *    prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Parts of the project are originally copyright (c) 2013-2015 The PurpleI2P Project
 */

#ifndef SRC_CORE_CRYPTO_X509_H_
#define SRC_CORE_CRYPTO_X509_H_

#include <map>
#include <memory>
#include <sstream>
#include <string>

#include "Identity.h"  // i2p::data::Tag

namespace i2p {
namespace crypto {

// Placed here to use across implementations
typedef i2p::data::Tag<512> PublicKey;

/**
 * @class X509
 * @brief Processes X.509 certificate
 * @defailts Currently only PEM is supported
 */
class X509 {
 public:
  X509();
  ~X509();

  /// @brief Signing keys to return
  std::map<std::string, PublicKey> m_SigningKeys;

  /// @brief Acquires public signing key from PEM formatted X.509
  /// @return Signing key
  const std::map<std::string, PublicKey> GetSigningKey(
      std::stringstream& certificate);

 private:
  class X509Impl;
  std::unique_ptr<X509Impl> m_X509Pimpl;
};

}  // namespace crypto
}  // namespace i2p

#endif  // SRC_CORE_CRYPTO_X509_H_