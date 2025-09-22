----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 09/21/2025 11:16:56 PM
-- Design Name: 
-- Module Name: two_bit_adder - Structural
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity two_bit_adder_struct is
    Port (
    A    : in  STD_LOGIC_VECTOR(1 downto 0);
    B    : in  STD_LOGIC_VECTOR(1 downto 0);
    Cin  : in  STD_LOGIC;
    S    : out STD_LOGIC_VECTOR(1 downto 0);
    Cout : out STD_LOGIC
);
end two_bit_adder_struct;

architecture Structural of two_bit_adder_struct is

component full_adder_struct is
    Port (
        Cin  : in  STD_LOGIC;
        A    : in  STD_LOGIC;
        B    : in  STD_LOGIC;
        S    : out STD_LOGIC;
        Cout : out STD_LOGIC
    );
end component;

signal C1 : STD_LOGIC;  -- перенос между младшим и старшим разрядом
begin
    FA0: full_adder_struct
        port map (
            Cin  => Cin,
            A    => A(0),
            B    => B(0),
            S    => S(0),
            Cout => C1
        );
    FA1: full_adder_struct
        port map (
            Cin  => C1,
            A    => A(1),
            B    => B(1),
            S    => S(1),
            Cout => Cout
        );
end Structural;
