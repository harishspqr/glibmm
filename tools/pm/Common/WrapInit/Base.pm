# -*- mode: perl; perl-indent-level: 2; indent-tabs-mode: nil -*-
# gmmproc - Common::WrapInit::Base module
#
# Copyright 2012 glibmm development team
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
#

package Common::WrapInit::Base;

use strict;
use warnings;

sub _get_line
{
# TODO: error - not implemented.
  die;
}

sub new
{
  my ($type, $c_includes, $cxx_includes, $deprecated, $not_for_windows, $mm_module) = @_;
  my $class = (ref $type or $type or 'Common::WrapInit::Base');
  my $self =
  {
    'c_includes' => $c_includes,
    'cxx_includes' => $cxx_includes,
    'deprecated' => $deprecated,
    'not_for_windows' => $not_for_windows,
    'mm_module' => $mm_module
  };

  return bless $self, $class;
}

sub get_c_includes
{
  my ($self) = @_;

  return $self->{'c_includes'};
}

sub get_cxx_includes
{
  my ($self) = @_;

  return $self->{'cxx_includes'};
}

sub get_deprecated
{
  my ($self) = @_;

  return $self->{'deprecated'};
}

sub get_not_for_windows
{
  my ($self) = @_;

  return $self->{'not_for_windows'};
}

sub get_main_line
{
  my ($self) = @_;
  my $deprecated = $self->get_deprecated ();
  my $not_for_windows = $self->get_not_for_windows ();
  my $mm_module = $self->{'mm_module'};
  my $deprecation_macro = join ('', uc ($mm_module), '_DISABLE_DEPRECATED');
  my @lines = ();
  my $main_line = '';

  if ($deprecated)
  {
    push (@lines, join ('', '#ifndef ', $deprecation_macro, "\n"));
  }
  if ($not_for_windows)
  {
    push (@lines, '#ifndef G_OS_WIN32');
  }
  push (@lines, $self->_get_line ());
  if ($not_for_windows)
  {
    push (@lines, '#endif // G_OS_WIN32');
  }
  if ($deprecated)
  {
    push (@lines, join ('', '#endif // ', $deprecation_macro));
  }

  return join ("\n", @lines);
}

1; # indicate proper module load.
